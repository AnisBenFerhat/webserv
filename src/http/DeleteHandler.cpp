/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:06 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/06 10:47:24 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/DeleteHandler.hpp"

#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstring>

#include "errors/ErrorPageGenerator.hpp"
#include "http/HttpStatus.hpp"
#include "utils/Logger.hpp"

DeleteHandler::DeleteHandler(const Config& config,
							 const LocationBlock& location)
	: _config(config), _location(location) {
}

DeleteHandler::DeleteHandler(const DeleteHandler& other)
	: _config(other._config), _location(other._location) {
}

DeleteHandler& DeleteHandler::operator=(const DeleteHandler& other) {
	(void)other;
	return *this;
}

DeleteHandler::~DeleteHandler() {
}

HttpResponse DeleteHandler::createResponse(const HttpRequest& request) const {
	std::string uri = request.getPath();
	const std::string& locationPath = _location.getPath();

	if (uri.compare(0, locationPath.size(), locationPath) == 0)
		uri = uri.substr(locationPath.size());
	if (!uri.empty() && uri[0] == '/')
		uri = uri.substr(1);

	if (uri.find("..") != std::string::npos) {
		Logger::logWarning("403 - path traversal attempt blocked: [" + uri +
						   "]");
		return _generateError(HTTP_403_FORBIDDEN);
	}

	std::string rawPath = _location.getRoot();
	if (!rawPath.empty() && rawPath[rawPath.size() - 1] != '/' && !uri.empty())
		rawPath += '/';
	rawPath += uri;

	if (access(rawPath.c_str(), F_OK) != 0) {
		Logger::logWarning("404 - ressource not found: [" + rawPath + "]");
		return _generateError(HTTP_404_NOT_FOUND);
	}

	if (access(rawPath.c_str(), W_OK) != 0) {
		Logger::logWarning("403 - permission denied : [" + rawPath + "]");
		return _generateError(HTTP_403_FORBIDDEN);
	}

	if (unlink(rawPath.c_str()) != 0) {
		Logger::logWarning("500 - unlink() failed on: [" + rawPath + "] - " +
						   std::string(strerror(errno)));
		return _generateError(HTTP_500_INTERNAL_SERVER_ERROR);
	}

	Logger::logInfo("File deleted: [" + rawPath + "]");

	HttpResponse response;
	response.setStatus(HTTP_204_NO_CONTENT);
	return response;
}

HttpResponse DeleteHandler::_generateError(HttpStatus status) const {
	ErrorPageGenerator generator(_config);
	return generator.createResponse(status);
}
