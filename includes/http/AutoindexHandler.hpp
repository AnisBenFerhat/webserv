/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoindexHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 18:40:10 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/15 11:35:43 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEXHANDLER_HPP
#define AUTOINDEXHANDLER_HPP

#include "http/HttpResponse.hpp"
#include <string>
#include <vector>

/**
 * @brief Static utility class for generating HTML directory listings.
 *
 * Activated only when autoindex is enabled in the LocationBlock and the
 * requested resource is a directory with no index file.
 **/
class AutoindexHandler {
	public:
		/**
		 * @brief Generates a complete HTTP response with an HTML directory
		 * listing.
		 *
		 * Reads the directory at rootPath using opendir/readdir/closedir,
		 * sorts entries (directories first, then files, both alphabetically),
		 * and builds a navigable HTML page with correct href links.
		 * Returns a 403 Forbidden response if the directory cannot be opened.
		 *
		 * @param rootPath The physical path on disk (used for opendir and
		 * stat).
		 * @param uri      The virtual URI from the client request (used for
		 * href links).
		 * @return A fully populated HttpResponse ready to be serialized and
		 * sent.
		 **/
		static HttpResponse createResponse(const std::string& rootPath,
										   const std::string& uri);

	private:
		AutoindexHandler();
		AutoindexHandler(const AutoindexHandler& other);
		AutoindexHandler& operator=(const AutoindexHandler& other);
		~AutoindexHandler();

		static std::string _buildHtml(const std::string&			  uri,
									  const std::vector<std::string>& dirs,
									  const std::vector<std::string>& files);

		static std::string _getIcon(const std::string& filename);
};

#endif
