/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:43:28 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/26 21:53:43 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

/** *
 * @brief Enumeration of HTTP methods supported by the server.
 */
enum HttpMethod { HTTP_GET, HTTP_POST, HTTP_DELETE, HTTP_UNKNOWN };

class HttpRequest {
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);
		~HttpRequest();

		// Getters
		HttpMethod		   getMethod() const;
		const std::string& getPath() const;
		const std::string& getProtocol() const;
		const std::string& getBody() const;

		/**
		 * @brief Fetch a specific header value.
		 * @param key The header name.
		 * @return The header value, or an empty string if not found.
		 **/
		std::string getHeader(const std::string& key) const;

		/**
		 *@brief Returns a string representation of the HTTP method.
		 **/
		std::string getMethodString() const;

		// Setters
		void setMethod(HttpMethod method);
		void setPath(const std::string& path);
		void setProtocol(const std::string& protocol);
		void addHeader(const std::string& key, const std::string& value);
		void appendToBody(const std::string& content);

	private:
		HttpMethod						   _method;
		std::string						   _path;
		std::string						   _protocol;
		std::map<std::string, std::string> _headers;
		std::string						   _body;
};

#endif
