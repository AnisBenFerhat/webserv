/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:25:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/05 15:36:12 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>

#include "http/HttpStatus.hpp"

/** *
 * @brief Class designed to build and serialize an HTTP response.
 **/
class HttpResponse {
	public:
		HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);
		~HttpResponse();

		// Getters
		HttpStatus getStatus() const {
			return _status;
		}
		const std::string& getBody() const {
			return _body;
		};

		std::string getHeader(const std::string& key) const;

		// Setters
		void setStatus(HttpStatus status) {
			_status = status;
		}
		void setHeader(const std::string& key, const std::string& value) {
			_headers[key] = value;
		}
		void setBody(const std::string& body) {
			_body = body;
		}

		/**
		 * @brief Converts the object into a raw HTTP response string.
		 * @return A formatted string.
		 **/
		std::string serialize() const;

	private:
		HttpStatus						   _status;
		std::map<std::string, std::string> _headers;
		std::string						   _body;
};

#endif
