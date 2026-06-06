/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:53:43 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/04 17:31:27 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include <string>

#include "config/Config.hpp"
#include "config/LocationBlock.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

/**
 * @brief Handles HTTP POST requests for uploads and form data.
 */
class UploadHandler {
	public:
		UploadHandler(const Config& config, const LocationBlock& location);
		UploadHandler(const UploadHandler& other);
		UploadHandler& operator=(const UploadHandler& other);
		~UploadHandler();

		/**
		 * @brief Processes a POST request and returns the corresponding
		 * response.
		 *
		 * Validates the request body size and dispatches the body to the
		 * appropriate parser according to the Content-Type header.
		 *
		 * @param request The incoming HTTP POST request.
		 * @return A complete HTTP response for the request.
		 */
		HttpResponse createResponse(const HttpRequest& request) const;

	private:
		const Config& _config;
		const LocationBlock& _location;

		HttpResponse _handleMultipart(const std::string& body,
									  const std::string& boundary) const;
		HttpResponse _handleUrlEncoded() const;
		HttpResponse _processPart(const std::string& partHeaders,
								  const std::string& partBody) const;
		std::string _extractFilename(const std::string& partHeaders) const;
		HttpResponse _buildCreatedResponse() const;
		std::string _extractBoundary(const std::string& contentType) const;
		std::string _sanitizeFilename(const std::string& filename) const;
		bool _writeFile(const std::string& filename,
						const std::string& content) const;
		HttpResponse _generateError(HttpStatus status) const;
};

#endif
