/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 20:27:48 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/12 13:46:58 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include "http/HttpResponse.hpp"
#include <string>

/**
 * @brief Serves static files as HTTP responses.
 *
 * Reads the file in binary mode, sets the right MIME type, and fills the
 * response headers and body.
 */
class StaticFileHandler {
	public:
		StaticFileHandler();
		StaticFileHandler(const StaticFileHandler& other);
		StaticFileHandler& operator=(const StaticFileHandler& other);
		~StaticFileHandler();

		/**
		 * @brief Builds a 200 OK HttpResponse with the content of the given
		 * file.
		 *
		 * Reads the file in binary mode, resolves the MIME type from the
		 * extension, and populates Content-Type and Content-Length headers
		 * accordingly. Returns an empty-body response if the file cannot be
		 * opened.
		 *
		 * @param path The full path to the file on disk.
		 * @return A populated HttpResponse ready to be serialized and sent.
		 */
		HttpResponse createResponse(const std::string& path) const;

	private:
		std::string _readFile(const std::string& path) const;
};

#endif
