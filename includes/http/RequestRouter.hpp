/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestRouter.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 09:32:01 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/25 17:30:16 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTROUTER_HPP
#define REQUESTROUTER_HPP

#include <string>

#include "config/Config.hpp"
#include "config/LocationBlock.hpp"
#include "http/HttpRequest.hpp"

/**
 * @brief Logic engine responsible for matching an HTTP request to a specific
 * route.
 * Implements the "Longest Prefix Match" algorithm to select the most
 * specific LocationBlock from a given Config.
 **/
class RequestRouter {
	public:
		RequestRouter();
		RequestRouter(const RequestRouter& other);
		RequestRouter& operator=(const RequestRouter& other);
		~RequestRouter();

		/**
		 * @brief Find the best matching LocationBlock for a given request.
		 *
		 * @param server The server block containing potential routes.
		 * @param request The incoming client request.
		 * @return Pointer to the best LocationBlock, or NULL if no match is
		 * found.
		 */
		static const LocationBlock* matchLocation(const Config&		 server,
												  const HttpRequest& request);

	private:
		/**
		 * @brief Extracts the clean path from an URI (removes query strings).
		 */
		static std::string _getCleanPath(const std::string& uri);
};

#endif
