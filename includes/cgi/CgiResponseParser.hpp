/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiResponseParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 15:04:15 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/20 22:39:25 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIRESPONSEPARSER_HPP
#define CGIRESPONSEPARSER_HPP

#include "http/HttpResponse.hpp"
#include <string>

/**
 * @brief Static utility class that transforms raw CGI script output
 *        into a complete HttpResponse object.
 *
 * Parses CGI headers (Status, Content-Type, etc.), separates them from
 * the body, and maps the result to an HttpResponse. Returns 502 Bad Gateway
 * if the output is malformed or missing mandatory headers.
 **/
class CgiResponseParser {
	public:
		/**
		 * @brief Parses the raw stdout output of a CGI script.
		 *
		 * Splits the output at the first blank line to separate CGI headers
		 * from the body. Extracts the Status header to set the HTTP status
		 * code. Propagates all other headers to the HttpResponse.
		 * Returns 502 Bad Gateway on any structural failure.
		 *
		 * @param rawCgiOutput The full stdout captured from the CGI
		 * process.
		 * @return A fully populated HttpResponse ready to be serialized.
		 **/
		static HttpResponse createResponse(const std::string& rawCgiOutput);

	private:
		CgiResponseParser();
		CgiResponseParser(const CgiResponseParser& other);
		CgiResponseParser& operator=(const CgiResponseParser& other);
		~CgiResponseParser();

		static HttpResponse _makeBadGateway(const std::string& reason);
		static int			_parseStatusCode(const std::string& statusValue);
};

#endif
