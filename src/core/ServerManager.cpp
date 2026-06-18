/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 22:59:27 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/17 15:33:43 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/ServerManager.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "config/ConfigParser.hpp"
#include "errors/ErrorCode.hpp"
#include "net/ClientConnection.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

volatile std::sig_atomic_t g_keepRunning = 1;

void ServerManager::_signalHandler(int signum) {
	(void)signum;
	g_keepRunning = 0;
	Logger::logInfo("SIGINT signal detected, stopping the server");
}

void ServerManager::launch(const std::string& configFilePath) {
	_init(configFilePath);
	_run();
	_stop();
}

void ServerManager::checkTimeouts() {
	time_t current = time(NULL);

	std::map<int, ClientConnection*> clientCopy(
		_poller.getLookupTable().getClientBeginIt(),
		_poller.getLookupTable().getClientEndIt());

	std::map<int, ClientConnection*>::iterator it = clientCopy.begin();
	while (it != clientCopy.end()) {
		ClientConnection* client   = it->second;
		int				  clientFd = it->first;

		if (_poller.getLookupTable().getClientIt(clientFd) ==
			_poller.getLookupTable().getClientEndIt()) {
			++it;
			continue;
		}
		double idle = difftime(current, client->getLastActive());

		if (idle > static_cast<double>(_maxIdleThreshold)) {
			Logger::logInfo("Client FD [" + Convertor::intToStr(clientFd) +
							"] timed out after " +
							Convertor::intToStr(static_cast<int>(idle)) +
							"seconds idle. Removing Client.");
			_poller.removeClient(clientFd);
		}
		++it;
	}
}

void ServerManager::_init(const std::string& configFilePath) {
	struct stat uploadDir;
	if (stat("www/uploads", &uploadDir) != 0) {
		if (mkdir("www/uploads", 0755) != 0) {
			Logger::logWarning("Failed to create www/uploads: " +
								std::string(strerror(errno)));
		}
	}
	else if (!S_ISDIR(uploadDir.st_mode)) {
		Logger::logWarning("www/uploads exists, but it not a directory");
	}

	Logger::logPart("Parsing of the .conf");
	_configs = ConfigParser::parseConfig(configFilePath);
	Logger::logInfo("Total of server configuration(s) found [" +
					Convertor::uIntToStr(_configs.size()) + "]");

	std::ofstream sessionFile("www/data/sessions.json");
	if (sessionFile.is_open()) {
		sessionFile << "{}\n";
		sessionFile.close();
		Logger::logInfo("Session cleared on startup");
	}

	Logger::logPart("Initialization");
	_serverBlocks = ServerBlock::initServerBlocks(_configs);
	Logger::logInfo("Total of server(s) initialized [" +
					Convertor::uIntToStr(_serverBlocks.size()) + "]");
}

void ServerManager::_serverLoop() {
	_poller.initPoller(_serverBlocks);

	std::signal(SIGINT, _signalHandler);

	while (g_keepRunning == 1) {
		int activity = _poller.pollEvents(100);

		if (activity > 0) {
			_poller.dispatchActivity();
		}
		std::map<int, ClientConnection*> clientCopy(
			_poller.getLookupTable().getClientBeginIt(),
			_poller.getLookupTable().getClientEndIt());
		time_t current = time(NULL);
		for (std::map<int, ClientConnection*>::const_iterator it =
				 clientCopy.begin();
			 it != clientCopy.end(); ++it) {
			if (_poller.getLookupTable().getClientIt(it->first) ==
				_poller.getLookupTable().getClientEndIt()) {
				continue;
			}
			it->second->cgiTimeout(current, 15);
		}
		pid_t pidExited;
		int	  status;
		while ((pidExited = waitpid(-1, &status, WNOHANG)) > 0) {
			Logger::logInfo("Reap child CGI process [PID: " +
							Convertor::intToStr(pidExited) + "]");
		}
		checkTimeouts();
	}
}

void ServerManager::_run() {
	Logger::logPart("Launching");
	if (_configs.size()) {
		Logger::logInfo("Webserv is running with [" +
						Convertor::uIntToStr(_configs.size()) +
						"] server configuration(s)");
		_serverLoop();
	} else {
		Logger::logWarning(
			"Webserv needs at least one server to run, exiting ...");
		exit(1);
	}
}

void ServerManager::_stop() {
	Logger::logPart("Stopping");
	if (_configs.size()) {
		_poller.clearTable();
		Logger::logInfo("Webserv has been stopped");
	}
}

ServerManager::ServerManager()
	: _configs(), _serverBlocks(), _maxIdleThreshold(30) {}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < _serverBlocks.size(); ++i) {
		delete _serverBlocks[i];  // Triggers ~ServerBlock() -> ~TcpListener()
								  // -> close()
	}
	_serverBlocks.clear();
	Logger::logInfo("ServerManager cleanly deallocated all resources.");
}
