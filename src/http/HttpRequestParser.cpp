/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:08:17 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 15:44:03 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpRequestParser.hpp"
#include <sstream>
#include <cstdlib>

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::HttpRequestParser(const HttpRequestParser& other) {
	(void)other;
}

HttpRequestParser& HttpRequestParser::operator=(
	const HttpRequestParser& other) {
	(void)other;
	return *this;
}

HttpRequestParser::~HttpRequestParser() {}

HttpRequestParser::ParseResult HttpRequestParser::parse(
	const std::vector<char>& buffer, HttpRequest& request) {
	std::string raw(buffer.begin(), buffer.end());

	const std::string separator	   = "\r\n\r\n";
	size_t			  separatorPos = raw.find(separator);
	if (separatorPos == std::string::npos)
		return INCOMPLETE;

	std::string headerSection = raw.substr(0, separatorPos);
	std::string body		  = raw.substr(separatorPos + separator.size());

	std::istringstream stream(headerSection);
	std::string		   line;
	bool			   firstLine = true;

	while (std::getline(stream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (firstLine) {
			if (!_parseRequestLine(line, request))
				return ERROR;
			firstLine = false;
		} else {
			if (!line.empty() && !_parseHeaderLine(line, request))
				return ERROR;
		}
	}

	std::string contentLengthStr = request.getHeader("Content-Length");
	if (!contentLengthStr.empty()) {
		size_t expectedLength =
			static_cast<size_t>(std::atoi(contentLengthStr.c_str()));
		if (body.size() < expectedLength)
			return INCOMPLETE;
		request.appendToBody(body.substr(0, expectedLength));
	}

	return COMPLETE;
}

bool HttpRequestParser::_parseRequestLine(const std::string& line,
										  HttpRequest&		 request) {
	std::istringstream iss(line);
	std::string		   methodStr, path, protocol;

	if (!(iss >> methodStr >> path >> protocol))
		return false;

	if (methodStr == "GET")
		request.setMethod(HTTP_GET);
	else if (methodStr == "POST")
		request.setMethod(HTTP_POST);
	else if (methodStr == "DELETE")
		request.setMethod(HTTP_DELETE);
	else
		request.setMethod(HTTP_UNKNOWN);

	request.setPath(path);
	request.setProtocol(protocol);
	return true;
}

bool HttpRequestParser::_parseHeaderLine(const std::string& line,
										 HttpRequest&		request) {
	size_t colonPos = line.find(':');
	if (colonPos == std::string::npos)
		return false;

	std::string key	  = line.substr(0, colonPos);
	std::string value = line.substr(colonPos + 1);

	size_t valueStart = value.find_first_not_of(" \t");
	if (valueStart != std::string::npos)
		value = value.substr(valueStart);

	request.addHeader(key, value);
	return true;
}
