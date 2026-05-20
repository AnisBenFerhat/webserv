/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiResponseParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 15:04:26 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/20 23:56:10 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi/CgiResponseParser.hpp"
#include "http/HttpResponse.hpp"
#include <sstream>
#include <map>

HttpResponse CgiResponseParser::createResponse(
	const std::string& rawCgiOutput) {
	if (rawCgiOutput.empty())
		return _makeBadGateway("CGI script produced no output");

	size_t separatorLen = 4;
	size_t separator	= rawCgiOutput.find("\r\n\r\n");
	if (separator == std::string::npos) {
		separator	 = rawCgiOutput.find("\n\n");
		separatorLen = 2;
	}

	if (separator == std::string::npos)
		return _makeBadGateway("CGI output has no header/body separator");

	std::string headerBlock = rawCgiOutput.substr(0, separator);
	std::string body		= rawCgiOutput.substr(separator + separatorLen);

	std::map<std::string, std::string> cgiHeaders;
	std::istringstream				   stream(headerBlock);
	std::string						   line;

	while (std::getline(stream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;

		std::string key	  = line.substr(0, colon);
		std::string value = line.substr(colon + 1);

		size_t start = value.find_first_not_of(" \t");
		if (start != std::string::npos)
			value = value.substr(start);

		cgiHeaders[key] = value;
	}

	if (cgiHeaders.find("Content-Type") == cgiHeaders.end())
		return _makeBadGateway(
			"CGI output missing mandatory Content-Type header");

	HttpStatus										   status = HTTP_200_OK;
	std::map<std::string, std::string>::const_iterator iter =
		cgiHeaders.find("Status");
	if (iter != cgiHeaders.end()) {
		int code = _parseStatusCode(iter->second);
		status	 = static_cast<HttpStatus>(code);
	}

	HttpResponse response;
	response.setStatus(status);

	for (std::map<std::string, std::string>::const_iterator header =
			 cgiHeaders.begin();
		 header != cgiHeaders.end(); ++header) {
		if (header->first != "Status")
			response.setHeader(header->first, header->second);
	}

	std::ostringstream contentLength;
	contentLength << body.size();
	response.setHeader("Content-Length", contentLength.str());
	response.setBody(body);

	return response;
}

HttpResponse CgiResponseParser::_makeBadGateway(const std::string& reason) {
	std::string body =
		"<!DOCTYPE html>\n"
		"<html><head><title>502 Bad Gateway</title></head>\n"
		"<body><h1>502 Bad Gateway</h1><p>" +
		reason +
		"</p>"
		"<p>Webserv</p></body></html>\n";

	std::ostringstream contentLength;
	contentLength << body.size();

	HttpResponse response;
	response.setStatus(HTTP_502_BAD_GATEWAY);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", contentLength.str());
	response.setBody(body);
	return response;
}

int CgiResponseParser::_parseStatusCode(const std::string& statusValue) {
	std::istringstream iss(statusValue);
	int				   code = 200;
	iss >> code;

	if (code < 100 || code > 599)
		return 200;

	return code;
}
