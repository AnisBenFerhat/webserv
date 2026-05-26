/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 12:24:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/26 16:38:00 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <string>
#include <vector>

#include "config/Config.hpp"
#include "net/TcpListener.hpp"

class Config;

/**
 * @brief Manages a group of virtual servers sharing the same listening port.
 *
 * A ServerBlock binds a physical TcpListener to one or more Config objects.
 * It handles the logic of routing incoming requests to the correct Config
 * based on the 'Host' header of the HTTP request.
 */
class ServerBlock {
	public:
		// --- Manager Methods ---

		/**
		 * @brief Factory method to group Configs by port into
		 * ServerBlockManagers.
		 * @param configs A flat vector of all parsed configurations.
		 * @return A vector of ServerBlockManagers ready for socket
		 * initialization.
		 */
		static std::vector<ServerBlock*> initServerBlocks(
			const std::vector<Config>& configs);

		bool startNetwork(int port);

		// --- Getters ---

		/**
		 * @brief Matches a hostname to a specific virtual server configuration.
		 * @param hostname The domain name from the HTTP Host header.
		 * @return Pointer to the matching Config, or a default if no match is
		 * found.
		 */
		const Config* getConfigForHost(const std::string& hostname) const;

		const std::vector<const Config*>& getConfigs() const {
			return _configsREF;
		}
		const TcpListener& getTcpListener() const {
			return _tcpListener;
		}
		int getSocket() const {
			return _tcpListener.getSocket();
		}

		// --- Setters / Logic ---

		void addConfig(const Config* config) {
			if (config) {
				_configsREF.push_back(config);
			}
		}
		void setConfigs(std::vector<const Config*> configs) {
			_configsREF = configs;
		}
		void setTcpListener(const TcpListener tcpListener) {
			_tcpListener = tcpListener;
		}

		// --- Constructors / Destructor

		ServerBlock();
		ServerBlock(const ServerBlock& other);
		ServerBlock& operator=(const ServerBlock& other);
		ServerBlock(const Config* singleConfig);
		~ServerBlock();

	private:
		/// @brief References to Configs sharing this same port.
		std::vector<const Config*> _configsREF;

		/// @brief The network listener for this block's port.
		TcpListener _tcpListener;
};
#endif
