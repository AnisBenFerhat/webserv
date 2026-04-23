/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 19:19:40 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/22 13:10:07 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <string>

/**
 * @brief Enumeration of all HTTP status codes supported by the server.
 *
 * Values are explicitly set to their numeric HTTP code for safe integer
 * conversion.
 * Use the HTTP_ prefix to prevent name conflicts.
 */
enum HttpStatus {
	HTTP_200_OK							= 200,
	HTTP_201_CREATED					= 201,
	HTTP_204_NO_CONTENT					= 204,
	HTTP_301_MOVED_PERMANENTLY			= 301,
	HTTP_302_FOUND						= 302,
	HTTP_400_BAD_REQUEST				= 400,
	HTTP_401_UNAUTHORIZED				= 401,
	HTTP_403_FORBIDDEN					= 403,
	HTTP_404_NOT_FOUND					= 404,
	HTTP_405_METHOD_NOT_ALLOWED			= 405,
	HTTP_408_REQUEST_TIMEOUT			= 408,
	HTTP_409_CONFLICT					= 409,
	HTTP_411_LENGTH_REQUIRED			= 411,
	HTTP_413_PAYLOAD_TOO_LARGE			= 413,
	HTTP_414_URI_TOO_LONG				= 414,
	HTTP_415_UNSUPPORTED_MEDIA_TYPE		= 415,
	HTTP_500_INTERNAL_SERVER_ERROR		= 500,
	HTTP_501_NOT_IMPLEMENTED			= 501,
	HTTP_502_BAD_GATEWAY				= 502,
	HTTP_504_GATEWAY_TIMEOUT			= 504,
	HTTP_505_HTTP_VERSION_NOT_SUPPORTED = 505
};

/**
 * @brief Returns the official HTTP reason phrase for a given status code.
 * @param status The HTTP status code enum value.
 * @return The standard reason phrase string (e.g. "OK", "Not Found").
 */
std::string getReasonPhrase(HttpStatus status);

/**
 * @brief Converts an HttpStatus enum value to its integer representation.
 * @param status The HTTP status code enum value.
 * @return The numeric HTTP status code (e.g. 200, 404).
 */
int httpStatusToInt(HttpStatus status);

#endif
