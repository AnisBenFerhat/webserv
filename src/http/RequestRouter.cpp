/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestRouter.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 09:32:16 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/05 12:13:17 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/RequestRouter.hpp"

RequestRouter::RequestRouter() {}

RequestRouter::RequestRouter(const RequestRouter& other) {
	*this = other;
}

RequestRouter& RequestRouter::operator=(const RequestRouter& other) {
	(void)other;
	return *this;
}

RequestRouter::~RequestRouter() {}

const LocationBlock* RequestRouter::matchLocation(const ServerBlock& server,
												  const HttpRequest& request) {
	std::string cleanPath = _getCleanPath(request.getPath());
	const std::vector<LocationBlock>& locations		  = server.getLocations();
	const LocationBlock*			  bestMatch		  = NULL;
	size_t							  longestMatchLen = 0;

	for (size_t i = 0; i < locations.size(); ++i) {
		const std::string& locationPath = locations[i].getPath();

		if (cleanPath.compare(0, locationPath.length(), locationPath) == 0) {
			if (locationPath.length() > longestMatchLen) {
				longestMatchLen = locationPath.length();
				bestMatch		= &locations[i];
			}
		}
	}
	return bestMatch;
}

std::string RequestRouter::_getCleanPath(const std::string& uri) {
	size_t queryPosition = uri.find('?');

	if (queryPosition != std::string::npos)
		return uri.substr(0, queryPosition);

	return uri;
}
