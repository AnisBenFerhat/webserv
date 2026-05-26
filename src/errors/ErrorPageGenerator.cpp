/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageGenerator.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 11:58:14 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/25 17:35:35 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/ErrorPageGenerator.hpp"

#include <fstream>
#include <sstream>

#include "http/HttpStatus.hpp"

ErrorPageGenerator::ErrorPageGenerator(const Config& config)
	: _config(config) {}

ErrorPageGenerator::ErrorPageGenerator(const ErrorPageGenerator& other)
	: _config(other._config) {}

ErrorPageGenerator& ErrorPageGenerator::operator=(
	const ErrorPageGenerator& other) {
	(void)other;
	return *this;
}

ErrorPageGenerator::~ErrorPageGenerator() {}

HttpResponse ErrorPageGenerator::createResponse(HttpStatus status) const {
	HttpResponse response;

	response = _tryCustomPage(status);
	if (!response.getBody().empty()) return response;

	response = _tryDefaultFile(status);
	if (!response.getBody().empty()) return response;

	return _generateFallback(status);
}

HttpResponse ErrorPageGenerator::_tryCustomPage(HttpStatus status) const {
	const std::map<int, std::string>& errorPages = _config.getErrorPages();
	std::map<int, std::string>::const_iterator iter =
		errorPages.find(httpStatusToInt(status));

	if (iter == errorPages.end()) return HttpResponse();

	std::string body = _readFile(iter->second);
	if (body.empty()) return HttpResponse();

	return _buildResponse(status, body);
}

HttpResponse ErrorPageGenerator::_tryDefaultFile(HttpStatus status) const {
	std::ostringstream oss;
	oss << "www/errors/" << httpStatusToInt(status) << ".html";

	std::string body = _readFile(oss.str());
	if (body.empty()) return HttpResponse();

	return _buildResponse(status, body);
}

HttpResponse ErrorPageGenerator::_generateFallback(HttpStatus status) const {
	std::ostringstream oss;
	int				   code	  = httpStatusToInt(status);
	std::string		   reason = getReasonPhrase(status);

	oss << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>" << code << " " << reason << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << code << " " << reason << "</h1>\n"
		<< "<hr><p>Webserv</p>\n"
		<< "</body>\n"
		<< "</html>\n";

	return _buildResponse(status, oss.str());
}

std::string ErrorPageGenerator::_readFile(const std::string& path) const {
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) return "";

	std::ostringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

HttpResponse ErrorPageGenerator::_buildResponse(HttpStatus		   status,
												const std::string& body) const {
	std::ostringstream contentLength;
	contentLength << body.size();

	HttpResponse response;

	response.setStatus(status);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", contentLength.str());

	response.setBody(body);
	return response;
}
