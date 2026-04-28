/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:43:41 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/26 21:52:07 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpRequest.hpp"

HttpRequest::HttpRequest()
	: _method(HTTP_UNKNOWN), _path(""), _protocol(""), _body("") {}

HttpRequest::HttpRequest(const HttpRequest& other) {
	*this = other;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
	if (this != &other) {
		this->_method	= other._method;
		this->_path		= other._path;
		this->_protocol = other._protocol;
		this->_headers	= other._headers;
		this->_body		= other._body;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

// Getters
HttpMethod HttpRequest::getMethod() const {
	return _method;
}

const std::string& HttpRequest::getPath() const {
	return _path;
}

const std::string& HttpRequest::getProtocol() const {
	return _protocol;
}

const std::string& HttpRequest::getBody() const {
	return _body;
}

std::string HttpRequest::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator iter =
		_headers.find(key);
	if (iter != _headers.end()) {
		return iter->second;
	}
	return "";
}

std::string HttpRequest::getMethodString() const {
	switch (_method) {
		case HTTP_GET:
			return "GET";
		case HTTP_POST:
			return "POST";
		case HTTP_DELETE:
			return "DELETE";
		default:
			return "UNKNOWN";
	}
}

// Setters

void HttpRequest::setMethod(HttpMethod method) {
	_method = method;
}

void HttpRequest::setPath(const std::string& path) {
	_path = path;
}

void HttpRequest::setProtocol(const std::string& protocol) {
	_protocol = protocol;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void HttpRequest::appendToBody(const std::string& content) {
	_body += content;
}
