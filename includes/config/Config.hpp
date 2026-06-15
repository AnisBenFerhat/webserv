/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:16 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/12 14:04:39 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "config/LocationBlock.hpp"
#include "config/ServerBlock.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

#define DEFAULT_MAX_BODY_SIZE 10485760

/**
 * @brief Manages server configuration data storage.
 *
 * This class describes and store the full configuration of the product of
 * ConfigParser that extract all the informations written in a Server block from
 * the .conf extension file.
 */
class Config {
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		// --- Getters ---
		int getPort() const {
			return _port;
		}
		size_t getClientMaxBodySize() const {
			return _clientMaxBodySize;
		}
		const std::string& getHost() const {
			return _host;
		}
		const std::vector<std::string>& getServerBlockName() const {
			return _serverNames;
		}
		const std::map<int, std::string>& getErrorPages() const {
			return _errorPages;
		}
		const std::vector<LocationBlock>& getLocationBlocks() const {
			return _locations;
		}
		const std::string& getDefaultRoot() const {
			return _defaultRoot;
		}
		const std::string& getDefaultIndex() const {
			return _defaultIndex;
		}
		bool isLocationsEmpty() const {
			return _locations.empty();
		}
		bool isDefaultRootEmpty() const {
			return _defaultRoot.empty();
		}

		// --- Setters ---
		void setPort(int port) {
			_port = port;
		}
		void setHost(const std::string& host) {
			_host = host;
		}
		void setClientMaxBodySize(size_t size) {
			_clientMaxBodySize = size;
		}
		void addServerBlockName(const std::string& name) {
			_serverNames.push_back(name);
		}
		void addErrorPage(int code, const std::string& path) {
			_errorPages[code] = path;
		}
		void addLocationBlock(const LocationBlock& loc) {
			_locations.push_back(loc);
		}
		void setDefaultRoot(const std::string& root) {
			_defaultRoot = root;
		}
		void setDefaultIndex(const std::string& index) {
			_defaultIndex = index;
		}

		// --- Booleans ---
		bool hasServerBlockName(const std::string& name) const;

	private:
		// --- Private components ---

		size_t					   _clientMaxBodySize;
		std::string				   _defaultIndex;
		std::string				   _defaultRoot;
		std::map<int, std::string> _errorPages;
		std::string				   _host;
		std::vector<LocationBlock> _locations;
		int						   _port;  ///< @brief [1024-65535] valid range.
		std::vector<std::string>   _serverNames;
};

#endif
