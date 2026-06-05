/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:07:52 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/04 11:39:41 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
#define HTTPREQUESTPARSER_HPP

#include "http/HttpRequest.hpp"
#include <vector>
#include <string>

class HttpRequestParser {
	public:
		enum ParseResult { INCOMPLETE, COMPLETE, ERROR };

		HttpRequestParser();
		HttpRequestParser(const HttpRequestParser& other);
		HttpRequestParser& operator=(const HttpRequestParser& other);
		~HttpRequestParser();

		/**
		 * @brief Parses a raw buffer into an HttpRequest.
		 *
		 * @param buffer The raw bytes received from the client socket.
		 * @param request The HttpRequest to populate.
		 * @return COMPLETE, INCOMPLETE, or ERROR.
		 */
		static ParseResult parse(const std::vector<char>& buffer,
								 HttpRequest&			  request,
								 std::size_t&			  bytesParsed);

	private:
		static bool _parseRequestLine(const std::string& line,
									  HttpRequest&		 request);
		static bool _parseHeaderLine(const std::string& line,
									 HttpRequest&		request);
};

#endif
