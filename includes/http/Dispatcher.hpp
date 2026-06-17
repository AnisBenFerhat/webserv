/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:07:49 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/17 15:32:53 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "config/Config.hpp"
#include "config/LocationBlock.hpp"
#include "errors/ErrorPageGenerator.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include <sys/stat.h>
#include <string>

/**
 * @brief Central dispatcher for HTTP requests.
 *
 * It checks the method, resolves the resource, and routes the request to the
 * right handler: CGI, static file, autoindex, or error response.
 */
class Dispatcher {
	public:
		/**
		 * @brief Resolves and dispatches an HTTP request to the correct
		 * handler.
		 * @param request  The fully parsed HTTP request.
		 * @param location The matched LocationBlock from RequestRouter.
		 * @param config   The active server Config - passed to
		 * ErrorPageGenerator.
		 * @return A fully populated HttpResponse ready to be serialized.
		 */
		static HttpResponse dispatch(const HttpRequest&	  request,
									 const LocationBlock& location,
									 const Config&		  config);

		/**
		 * @brief Determines if request path should be handled as a
		 * CGI request
		 * @param fullPath Absolute path
		 * @param location Matched LocationBlock
		 * @return true if request should be dispatched to CGI subprocess,
		 * otherwise false
		 */
		static bool			_isCgiRequest(const std::string&   fullPath,
										  const LocationBlock& location);

		/**
		 * @brief Resolves absolute file path for request URI against
		 * location block.
		 * @param request parsed HTTP request
		 * @param location matched LocationBlock
		 * @return absolute file path as a string
		 */
		static std::string	_resolvePath(const HttpRequest&	  request,
										 const LocationBlock& location);
	private:
		Dispatcher();
		Dispatcher(const Dispatcher& other);
		Dispatcher& operator=(const Dispatcher& other);
		~Dispatcher();

		static bool			_isMethodAllowed(const HttpRequest&	  request,
											 const LocationBlock& location);
		static std::string	_getExtension(const std::string& path);
		static HttpResponse _dispatchDirectory(const std::string&	fullPath,
											   const HttpRequest&	request,
											   const LocationBlock& location,
											   const Config&		config);
		static HttpResponse _dispatchFile(const std::string& fullPath);
		static HttpResponse _dispatchCgi(const HttpRequest&	  request,
										 const LocationBlock& location,
										 const std::string&	  scriptPath,
										 const Config&		  config);
		static HttpResponse _generateError(HttpStatus	 status,
										   const Config& config);
};

#endif
