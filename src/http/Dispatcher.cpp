/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:08:10 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/18 13:50:50 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Dispatcher.hpp"

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cgi/CgiHandler.hpp"
#include "cgi/CgiResponseParser.hpp"
#include "errors/ErrorPageGenerator.hpp"
#include "http/AutoindexHandler.hpp"
#include "http/DeleteHandler.hpp"
#include "http/HttpStatus.hpp"
#include "http/StaticFileHandler.hpp"
#include "http/UploadHandler.hpp"
#include "utils/Logger.hpp"

HttpResponse Dispatcher::dispatch(const HttpRequest& request,
								  const LocationBlock& location,
								  const Config& config) {
	if (!location.getMethods().empty() &&
		!_isMethodAllowed(request, location)) {
		Logger::logWarning("405 — method [" + request.getMethodString() +
						   "] not allowed on [" + location.getPath() + "]");
		return _generateError(HTTP_405_METHOD_NOT_ALLOWED, config);
	}

	if (request.getMethod() == HTTP_POST && !location.getUploadDir().empty()) {
		UploadHandler handler(config, location);
		return handler.createResponse(request);
	}

	if (request.getMethod() == HTTP_DELETE) {
		DeleteHandler handler(config, location);
		return handler.createResponse(request);
	}

	std::string fullPath = _resolvePath(request, location);
	Logger::logInfo("Dispatcher resolved path: [" + fullPath + "]");

	if (_isCgiRequest(fullPath, location)) {
		struct stat statInfo;
		if (stat(fullPath.c_str(), &statInfo) != 0) {
			Logger::logWarning("404 — CGI script not found: [" + fullPath +
							   "]");
			return _generateError(HTTP_404_NOT_FOUND, config);
		}
		return _dispatchCgi(request, location, fullPath, config);
	}

	struct stat statInfo;
	if (stat(fullPath.c_str(), &statInfo) != 0) {
		Logger::logWarning("404 — resource not found: [" + fullPath + "]");
		return _generateError(HTTP_404_NOT_FOUND, config);
	}

	if (S_ISDIR(statInfo.st_mode))
		return _dispatchDirectory(fullPath, request, location, config);
	if (S_ISREG(statInfo.st_mode))
		return _dispatchFile(fullPath);

	return _generateError(HTTP_403_FORBIDDEN, config);
}

bool Dispatcher::_isMethodAllowed(const HttpRequest& request,
								  const LocationBlock& location) {
	const std::vector<std::string>& methods = location.getMethods();
	const std::string& method = request.getMethodString();
	for (size_t i = 0; i < methods.size(); ++i) {
		if (methods[i] == method)
			return true;
	}
	return false;
}

bool Dispatcher::_isCgiRequest(const std::string& fullPath,
							   const LocationBlock& location) {
	const std::string& cgiExt = location.getCgiExtension();
	if (cgiExt.empty())
		return false;
	return _getExtension(fullPath) == cgiExt;
}

std::string Dispatcher::_getExtension(const std::string& path) {
	size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
		return "";
	size_t slashPos = path.rfind('/');
	if (slashPos != std::string::npos && dotPos < slashPos)
		return "";
	return path.substr(dotPos);
}

std::string Dispatcher::_resolvePath(const HttpRequest& request,
									 const LocationBlock& location) {
	const std::string& root = location.getRoot();
	const std::string& locationPath = location.getPath();
	std::string uri = request.getPath();

	size_t queryPos = uri.find('?');
	if (queryPos != std::string::npos)
		uri = uri.substr(0, queryPos);

	std::string relativePath = uri;
	if (uri.compare(0, locationPath.size(), locationPath) == 0)
		relativePath = uri.substr(locationPath.size());
	if (!relativePath.empty() && relativePath[0] == '/')
		relativePath = relativePath.substr(1);

	std::string fullPath = root;
	if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/' &&
		!relativePath.empty())
		fullPath += '/';
	fullPath += relativePath;

	return fullPath;
}

HttpResponse Dispatcher::_dispatchDirectory(const std::string& fullPath,
											const HttpRequest& request,
											const LocationBlock& location,
											const Config& config) {
	if (request.getPath()[request.getPath().size() - 1] != '/') {
		HttpResponse response;
		response.setStatus(HTTP_301_MOVED_PERMANENTLY);
		response.setHeader("Location", request.getPath() + "/");
		response.setHeader("Content-Length", "0");
		response.setHeader("Connection", "close");
		return response;
	}

	const std::string& indexFile = location.getIndex();
	if (!indexFile.empty()) {
		std::string indexPath = fullPath;
		if (indexPath[indexPath.size() - 1] != '/')
			indexPath += '/';
		indexPath += indexFile;

		struct stat indexStatInfo;
		if (stat(indexPath.c_str(), &indexStatInfo) == 0 &&
			S_ISREG(indexStatInfo.st_mode)) {
			StaticFileHandler handler;
			return handler.createResponse(indexPath);
		}
	}

	if (location.getAutoindex())
		return AutoindexHandler::createResponse(fullPath, request.getPath());

	Logger::logWarning("403 — directory listing forbidden: [" + fullPath + "]");
	return _generateError(HTTP_403_FORBIDDEN, config);
}

HttpResponse Dispatcher::_dispatchFile(const std::string& fullPath) {
	StaticFileHandler handler;
	return handler.createResponse(fullPath);
}

HttpResponse Dispatcher::_dispatchCgi(const HttpRequest& request,
									  const LocationBlock& location,
									  const std::string& scriptPath,
									  const Config& config) {
	const std::string& interpreter = location.getCgiInterpreter();

	if (interpreter.empty()) {
		Logger::logWarning("CGI interpreter not configured for: [" +
						   scriptPath + "]");
		return _generateError(HTTP_500_INTERNAL_SERVER_ERROR, config);
	}

	CgiHandler handler;
	int outputPipeFd =
		handler.launchCgiProcess(request, scriptPath, interpreter);

	if (outputPipeFd < 0) {
		Logger::logWarning("CgiHandler failed to launch: [" + scriptPath + "]");
		return _generateError(HTTP_502_BAD_GATEWAY, config);
	}

	std::string cgiOutput;
	char readBuffer[4096];
	ssize_t bytesRead;
	while ((bytesRead = read(outputPipeFd, readBuffer, sizeof(readBuffer))) > 0)
		cgiOutput.append(readBuffer, static_cast<size_t>(bytesRead));
	close(outputPipeFd);

	int childStatus;
	waitpid(handler.getPid(), &childStatus, 0);

	if (cgiOutput.empty()) {
		Logger::logWarning("CGI script produced no output: [" + scriptPath +
						   "]");
		return _generateError(HTTP_502_BAD_GATEWAY, config);
	}

	return CgiResponseParser::createResponse(cgiOutput);
}

HttpResponse Dispatcher::_generateError(HttpStatus status,
										const Config& config) {
	ErrorPageGenerator generator(config);
	return generator.createResponse(status);
}
