/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:43:28 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/25 18:00:28 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <map>
#include <string>

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
		HttpMethod getMethod() const {
			return _method;
		};
		const std::string& getPath() const {
			return _path;
		}
		const std::string& getProtocol() const {
			return _protocol;
		}
		const std::string& getBody() const {
			return _body;
		}

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
		void setMethod(HttpMethod method) {
			_method = method;
		};
		void setPath(const std::string& path) {
			_path = path;
		};
		void setProtocol(const std::string& protocol) {
			_protocol = protocol;
		};
		void addHeader(const std::string& key, const std::string& value) {
			_headers[key] = value;
		};
		void appendToBody(const std::string& content) {
			_body += content;
		};

	private:
		HttpMethod						   _method;
		std::string						   _path;
		std::string						   _protocol;
		std::map<std::string, std::string> _headers;
		std::string						   _body;
};

#endif
