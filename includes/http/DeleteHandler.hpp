/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:53:40 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/04 14:26:34 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETEHANDLER_HPP
#define DELETEHANDLER_HPP

#include <string>

#include "config/Config.hpp"
#include "config/LocationBlock.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

/**
 * @brief Handles HTTP DELETE requests for resource deletion.
 */
class DeleteHandler {
	public:
		DeleteHandler(const Config& config, const LocationBlock& location);
		DeleteHandler(const DeleteHandler& other);
		DeleteHandler& operator=(const DeleteHandler& other);
		~DeleteHandler();

		/**
		 * @brief Processes a DELETE request and returns the corresponding
		 * response.
		 *
		 * Resolves the target resource, validates access to it, and deletes it
		 * if the request is allowed.
		 *
		 * @param request The incoming HTTP DELETE request.
		 * @return The HTTP response generated for the request.
		 */
		HttpResponse createResponse(const HttpRequest& request) const;

	private:
		const Config& _config;
		const LocationBlock& _location;

		HttpResponse _generateError(HttpStatus status) const;
};

#endif
