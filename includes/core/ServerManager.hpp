/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 22:56:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/31 17:04:50 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <string>

#include "config/Config.hpp"
#include "config/ServerBlock.hpp"
#include "net/Poller.hpp"

/**
 * @brief The central controller of the web server.
 *
 * ServerManager coordinates the lifecycle of the application. It utilizes the
 * ConfigParser to build server blocks, initializes network listeners, and
 * manages the I/O multiplexing loop to handle concurrent client connections.
 */
class ServerManager {
	public:
		ServerManager();

		~ServerManager();

		/**
		 * @brief Starts the server from scratch using a config file.
		 *
		 * A wrapper method that calls _init(), _run() and then _stop().
		 * @param configFilePath Path to the server configuration file.
		 */
		void launch(const std::string& configFilePath);

	private:
		ServerManager(const ServerManager& other);
		ServerManager& operator=(const ServerManager& other);

		/**
		 * @brief Initializes the server internal state.
		 *
		 * Triggers the ConfigParser, validates the resulting configurations,
		 * and sets up the ServerBlocks and their respective sockets.
		 * @param configFilePath Path to the server configuration file.
		 */
		void _init(const std::string& configFilePath);

		/**
		 * @brief Enters the infinite event loop.
		 *
		 * This is where the multiplexor (e.g., epoll, poll, or select) monitors
		 * sockets for incoming data and dispatches events to the handlers.
		 */
		void _run();

		/*
		 * @brief Core loop of the Web Server.
		 */
		void _serverLoop();

		/**
		 * @brief Gracefully shuts down the server.
		 *
		 * Closes all active sockets, clears configuration data, and releases
		 * allocated resources.
		 */
		void _stop();

		/**
		 * @brief Modify the SIGKILL signal behavior.
		 *
		 * Switch the boolean used in the _run() method loop to make it stop.
		 */
		static void _signalHandler(int signum);

		std::vector<Config>		  _configs;
		Poller					  _poller;
		std::vector<ServerBlock*> _serverBlocks;
};

#endif
