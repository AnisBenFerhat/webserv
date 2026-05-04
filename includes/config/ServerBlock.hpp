/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 12:24:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/04 12:43:07 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <string>
#include <vector>
#include <map>
#include "config/LocationBlock.hpp"

#define DEFAULT_MAX_BODY_SIZE 1048576

/**
 * @brief Represents a single server block from the configuration.
 * Stores listening parameters, server names and error page mappings.
 **/
class ServerBlock {
	public:
		ServerBlock();
		ServerBlock(const ServerBlock& other);
		ServerBlock& operator=(const ServerBlock& other);
		~ServerBlock();

		// Getters
		int								  getPort() const;
		const std::string&				  getHost() const;
		const std::vector<std::string>&	  getServerNames() const;
		const std::map<int, std::string>& getErrorPages() const;
		size_t							  getClientMaxBodySize() const;
		const std::vector<LocationBlock>& getLocations() const;

		// Setters (for parser)
		void setPort(int port);
		void setHost(const std::string& host);
		void addServerName(const std::string& name);
		void addErrorPage(int code, const std::string& path);
		void setClientMaxBodySize(size_t size);
		void addLocation(const LocationBlock& location);

	private:
		int						   _port;
		std::string				   _host;
		std::vector<std::string>   _serverNames;
		std::map<int, std::string> _errorPages;
		size_t					   _clientMaxBodySize;
		std::vector<LocationBlock> _locations;
};

#endif
