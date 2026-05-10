/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:26:03 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/10 13:30:16 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse() : _status(HTTP_200_OK) {}

HttpResponse::HttpResponse(const HttpResponse& other) {
	*this = other;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		_status	 = other._status;
		_headers = other._headers;
		_body	 = other._body;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

HttpStatus HttpResponse::getStatus() const {
	return _status;
}

const std::string& HttpResponse::getBody() const {
	return _body;
}

void HttpResponse::setStatus(HttpStatus status) {
	_status = status;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
	_body = body;
}

std::string HttpResponse::serialize() const {
	std::ostringstream osstream;

	osstream << "HTTP/1.1 " << httpStatusToInt(_status) << " "
			 << getReasonPhrase(_status) << "\r\n";

	std::map<std::string, std::string>::const_iterator iter;
	for (iter = _headers.begin(); iter != _headers.end(); ++iter) {
		osstream << iter->first << ": " << iter->second << "\r\n";
	}

	osstream << "\r\n";
	osstream << _body;

	return osstream.str();
}
