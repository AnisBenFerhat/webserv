/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:26:03 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/05 15:36:35 by elkanega         ###   ########.fr       */
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

std::string HttpResponse::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator iter =
		_headers.find(key);
	if (iter != _headers.end()) {
		return iter->second;
	}
	return "";
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
