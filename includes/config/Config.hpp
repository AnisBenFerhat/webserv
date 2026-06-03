/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:16 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 12:30:52 by aben-fer         ###   ########.fr       */
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
#include "errors/ErrorCode.hpp"

#define DEFAULT_MAX_BODY_SIZE 1048576

/**
 * @brief Manages server configuration data from parsing to storage.
 *
 * This class opens the configuration file provided via command-line arguments
 * that is composed of `nginx-style` blocks, processes the `server {}` ones, and
 * extracts the necessary data into `Config` objects for the ServerManager.
 */
class Config {
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		/**
		 * @brief Parses the entire configuration file and returns server
		 * blocks.
		 *
		 * @param configFilePath The path to the `.conf` file to be parsed.
		 * @return A vector of populated Config objects.
		 * @throw May throw exceptions depending on file validity or syntax
		 * errors.
		 */
		static std::vector<Config> parseConfig(
			const std::string& configFilePath);

		// --- Booleans ---

		/**
		 * @brief Checks if a specific domain name exists in the server's
		 * configuration.
		 * @param name The domain name to search for.
		 * @return true if found, false otherwise.
		 */
		bool hasServerBlockName(const std::string& name) const;

		// --- Getters ---

		int	   getPort() const { return _port; }
		size_t getClientMaxBodySize() const { return _clientMaxBodySize; }
		const std::string&				getHost() const { return _host; }
		const std::vector<std::string>& getServerBlockName() const {
			return _serverNames;
		}
		const std::map<int, std::string>& getErrorPages() const {
			return _errorPages;
		}
		const std::vector<LocationBlock>& getLocationBlocks() const {
			return _locations;
		}

		/**
		 * @brief Returns the default root directory used as a fallback for
		 * location blocks.
		 * @return A constant reference to the default root directory string.
		 */
		const std::string& getDefaultRoot() const { return _defaultRoot; }

		/**
		 * @brief Returns the default index file used as a fallback for location
		 * blocks.
		 * @return A constant reference to the default index file string.
		 */
		const std::string& getDefaultIndex() const { return _defaultIndex; }

		// --- Setters ---

		void setPort(int port) { _port = port; }
		void setHost(const std::string& host) { _host = host; }
		void setClientMaxBodySize(size_t size) { _clientMaxBodySize = size; }
		void addServerBlockName(const std::string& name) {
			_serverNames.push_back(name);
		}
		void addErrorPage(int code, const std::string& path) {
			_errorPages[code] = path;
		}
		void addLocationBlock(const LocationBlock& loc) {
			_locations.push_back(loc);
		}

		/**
		 * @brief Sets the default root directory for the server.
		 * @param root The default root directory path.
		 */
		void setDefaultRoot(const std::string& root) { _defaultRoot = root; }

		/**
		 * @brief Sets the default index file for the server.
		 * @param index The default index file name.
		 */
		void setDefaultIndex(const std::string& index) {
			_defaultIndex = index;
		}

	private:
		// --- Private methods ---

		/**
		 * @brief Internal helper to open and validate the configuration file
		 * stream.
		 *
		 * @param configFile A reference to the ifstream to be opened.
		 * @return ErrorCode representing the success or specific failure of the
		 * operation.
		 */
		static ErrorCode _openConfigFile(std::ifstream&	   configFile,
										 const std::string configFilePath);

		/**
		 * @brief Parses a single 'server {}' block and populates a Config
		 * object.
		 *
		 * @param configFile The open file stream currently positioned at a
		 * server block.
		 * @param config The Config object to be populated with parsed data.
		 * @return ErrorCode representing the success or syntax error within the
		 * block.
		 */
		ErrorCode _parseServerConf(std::ifstream& configFile);

		// --- Private components ---

		/// @brief The port number the server listens on.
		int _port;
		/// @brief Maximum allowed size for client request bodies.
		size_t _clientMaxBodySize;
		/// @brief Map of HTTP error codes to custom HTML file paths.
		std::map<int, std::string> _errorPages;
		/// @brief The IP address or hostname.
		std::string _host;
		/// @brief List of route-specific configurations.
		std::vector<LocationBlock> _locations;
		/// @brief List of domain names associated with this server.
		std::vector<std::string> _serverNames;

		std::string
			_defaultRoot;  ///< Server-level root fallback for location blocks.
		std::string _defaultIndex;	///< Server-level index fallback for
									///< location blocks.
};

#endif
