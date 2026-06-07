/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:09 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/06 10:47:39 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/UploadHandler.hpp"

#include <fstream>
#include <sstream>

#include "errors/ErrorPageGenerator.hpp"
#include "http/HttpStatus.hpp"
#include "utils/Logger.hpp"

UploadHandler::UploadHandler(const Config& config,
							 const LocationBlock& location)
	: _config(config), _location(location) {
}

UploadHandler::UploadHandler(const UploadHandler& other)
	: _config(other._config), _location(other._location) {
}

UploadHandler& UploadHandler::operator=(const UploadHandler& other) {
	(void)other;
	return *this;
}

UploadHandler::~UploadHandler() {
}

HttpResponse UploadHandler::createResponse(const HttpRequest& request) const {
	const std::string& body = request.getBody();
	const std::string contentType = request.getHeader("Content-Type");

	if (body.size() > _config.getClientMaxBodySize()) {
		Logger::logWarning("413 — body exceeds client_max_body_size");
		return _generateError(HTTP_413_PAYLOAD_TOO_LARGE);
	}
	if (contentType.find("multipart/form-data") != std::string::npos) {
		std::string boundary = _extractBoundary(contentType);
		if (boundary.empty()) {
			Logger::logWarning(
				"400 - multipart boundary missing from Content-Type");
			return _generateError(HTTP_400_BAD_REQUEST);
		}
		return _handleMultipart(body, boundary);
	}
	if (contentType.find("application/x-www-form-urlencoded") !=
		std::string::npos)
		return _handleUrlEncoded();

	Logger::logWarning("415 - unsupported Content-Type: [" + contentType + "]");
	return _generateError(HTTP_415_UNSUPPORTED_MEDIA_TYPE);
}

HttpResponse UploadHandler::_handleMultipart(
	const std::string& body, const std::string& boundary) const {
	std::string delimiter = "--" + boundary;
	size_t searchPosition = body.find(delimiter);

	if (searchPosition == std::string::npos) {
		Logger::logWarning(
			"400 - multipart body does not contain boundary delimiter");
		return _generateError(HTTP_400_BAD_REQUEST);
	}
	while (searchPosition != std::string::npos) {
		searchPosition += delimiter.size();
		if (body.compare(searchPosition, 2, "--") == 0)
			break;

		if (body.compare(searchPosition, 2, "\r\n") == 0)
			searchPosition += 2;

		size_t headerEndPosition = body.find("\r\n\r\n", searchPosition);
		if (headerEndPosition == std::string::npos)
			break;

		std::string partHeaders =
			body.substr(searchPosition, headerEndPosition - searchPosition);
		size_t bodyStart = headerEndPosition + 4;
		size_t nextBoundaryPosition = body.find("\r\n" + delimiter, bodyStart);
		if (nextBoundaryPosition == std::string::npos)
			break;
		std::string partBody =
			body.substr(bodyStart, nextBoundaryPosition - bodyStart);

		HttpResponse partResult = _processPart(partHeaders, partBody);
		if (!partResult.getBody().empty())
			return partResult;

		searchPosition = body.find(delimiter, nextBoundaryPosition);
	}
	return _buildCreatedResponse();
}

HttpResponse UploadHandler::_processPart(const std::string& partHeaders,
										 const std::string& partBody) const {
	std::string filename = _extractFilename(partHeaders);

	if (filename.empty())
		return HttpResponse();

	std::string safeFilename = _sanitizeFilename(filename);
	if (safeFilename.empty()) {
		Logger::logWarning("400 - Filename is empty after sanitization");
		return _generateError(HTTP_400_BAD_REQUEST);
	}
	if (!_writeFile(safeFilename, partBody)) {
		Logger::logWarning("500 - failed to write file : [" + safeFilename +
						   "]");
		return _generateError(HTTP_500_INTERNAL_SERVER_ERROR);
	}
	Logger::logInfo("File uploaded [" + safeFilename + "]");
	return HttpResponse();
}

HttpResponse UploadHandler::_buildCreatedResponse() const {
	std::string responseBody =
		"<!DOCTYPE html><html><body><h1>201 Created</h1>"
		"<p>File uploaded successfully.</p></body></html>\n";
	std::ostringstream contentLength;
	contentLength << responseBody.size();

	HttpResponse response;
	response.setStatus(HTTP_201_CREATED);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", contentLength.str());
	response.setHeader("Connection", "close");
	response.setBody(responseBody);
	return response;
}

HttpResponse UploadHandler::_handleUrlEncoded() const {
	std::string responseBody =
		"<!DOCTYPE html><html><body><h1>201 Created</h1>"
		"<p>Form data received.</p></body></html>\n";
	std::ostringstream contentLength;
	contentLength << responseBody.size();

	HttpResponse response;
	response.setStatus(HTTP_201_CREATED);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", contentLength.str());
	response.setBody(responseBody);
	return response;
}

std::string UploadHandler::_extractBoundary(
	const std::string& contentType) const {
	size_t boundaryPosition = contentType.find("boundary=");
	if (boundaryPosition == std::string::npos)
		return "";
	return contentType.substr(boundaryPosition + 9);
}

std::string UploadHandler::_sanitizeFilename(
	const std::string& filename) const {
	size_t lastSlashPosition = filename.rfind('/');
	std::string safeBasename = (lastSlashPosition != std::string::npos)
								   ? filename.substr(lastSlashPosition + 1)
								   : filename;

	size_t firstNonDotPosition = safeBasename.find_first_not_of('.');
	if (firstNonDotPosition == std::string::npos)
		return "";
	return safeBasename.substr(firstNonDotPosition);
}

std::string UploadHandler::_extractFilename(
	const std::string& partHeaders) const {
	size_t filenamePosition = partHeaders.find("filename=\"");
	if (filenamePosition == std::string::npos)
		return "";
	filenamePosition += 10;
	size_t filenameEndPosition = partHeaders.find("\"", filenamePosition);
	if (filenameEndPosition == std::string::npos)
		return "";
	return partHeaders.substr(filenamePosition,
							  filenameEndPosition - filenamePosition);
}

bool UploadHandler::_writeFile(const std::string& filename,
							   const std::string& content) const {
	std::string fullPath = _location.getUploadDir() + "/" + filename;
	std::ofstream outputFile(fullPath.c_str(), std::ios::binary);
	if (!outputFile.is_open())
		return false;
	outputFile.write(content.c_str(),
					 static_cast<std::streamsize>(content.size()));
	return outputFile.good();
}

HttpResponse UploadHandler::_generateError(HttpStatus status) const {
	ErrorPageGenerator generator(_config);
	return generator.createResponse(status);
}
