/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:25:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/10 13:23:37 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "http/HttpStatus.hpp"
#include <string>
#include <map>

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
		HttpStatus		   getStatus() const;
		const std::string& getBody() const;

		// Setters
		void setStatus(HttpStatus status);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);

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
