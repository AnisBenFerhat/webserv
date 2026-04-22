/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 19:19:33 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/18 20:04:34 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpStatus.hpp"

std::string getReasonPhrase(HttpStatus status) {
	switch (status) {
		// 2xx Success
		case HTTP_200_OK:
			return "OK";
		case HTTP_201_CREATED:
			return "Created";
		case HTTP_204_NO_CONTENT:
			return "No Content";

		// 3xx Redirection
		case HTTP_301_MOVED_PERMANENTLY:
			return "Moved Permanently";
		case HTTP_302_FOUND:
			return "Found";

		// 4xx Client Error
		case HTTP_400_BAD_REQUEST:
			return "Bad Request";
		case HTTP_401_UNAUTHORIZED:
			return "Unauthorized";
		case HTTP_403_FORBIDDEN:
			return "Forbidden";
		case HTTP_404_NOT_FOUND:
			return "Not Found";
		case HTTP_405_METHOD_NOT_ALLOWED:
			return "Method Not Allowed";
		case HTTP_408_REQUEST_TIMEOUT:
			return "Request Timeout";
		case HTTP_409_CONFLICT:
			return "Conflict";
		case HTTP_411_LENGTH_REQUIRED:
			return "Length Required";
		case HTTP_413_PAYLOAD_TOO_LARGE:
			return "Payload Too Large";
		case HTTP_414_URI_TOO_LONG:
			return "URI Too Long";
		case HTTP_415_UNSUPPORTED_MEDIA_TYPE:
			return "Unsupported Media Type";

		// 5xx Server Error
		case HTTP_500_INTERNAL_SERVER_ERROR:
			return "Internal Server Error";
		case HTTP_501_NOT_IMPLEMENTED:
			return "Not Implemented";
		case HTTP_502_BAD_GATEWAY:
			return "Bad Gateway";
		case HTTP_504_GATEWAY_TIMEOUT:
			return "Gateway Timeout";
		case HTTP_505_HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";

		default:
			return "Unknown Status";
	}
}

int httpStatusToInt(HttpStatus status) {
	return static_cast<int>(status);
}
