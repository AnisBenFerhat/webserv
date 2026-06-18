/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 17:11:46 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/17 15:36:11 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/ServerBlock.hpp"

#include <map>

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

std::vector<ServerBlock*> ServerBlock::initServerBlocks(
	const std::vector<Config>& configs) {
	std::vector<ServerBlock*> serverBlocks;
	std::map<int, size_t>	  portToIdx;

	for (size_t i = 0; i < configs.size(); ++i) {
		int port = configs[i].getPort();

		if (portToIdx.find(port) == portToIdx.end()) {
			Logger::logInfo("Server configuration [" + Convertor::uIntToStr(i) +
							"] initiated a new port [" +
							Convertor::intToStr(port) + "]");
			ServerBlock* newServerBlock = new ServerBlock(&configs[i]);

			if (!newServerBlock->startNetwork(port)) {
				for (size_t j = 0; j < serverBlocks.size(); ++j)
					delete serverBlocks[j];
				delete newServerBlock;
				return std::vector<ServerBlock*>();
			}

			portToIdx[port] = serverBlocks.size();
			serverBlocks.push_back(newServerBlock);
		} else {
			Logger::logInfo("Server configuration [" + Convertor::uIntToStr(i) +
							"] is sharing the port [" +
							Convertor::intToStr(port) + "]");
			serverBlocks[portToIdx[port]]->addConfig(&configs[i]);
		}
	}
	return serverBlocks;
}

bool ServerBlock::startNetwork(int port) {
	_tcpListener.initTcp(port);

	if (_tcpListener.getSocket() < 0) {
		Logger::logError("ServerBlock failed to activate network on port " +
						 Convertor::intToStr(port));
		return false;
	}
	return true;
}

// --- Getters ---

const Config* ServerBlock::getConfigForHost(const std::string& hostname) const {
	std::string cleanHost = hostname;
	size_t		colonPos  = cleanHost.find(':');
	if (colonPos != std::string::npos) {
		cleanHost = cleanHost.substr(0, colonPos);
	}

	for (size_t i = 0; i < cleanHost.length(); ++i) {
		cleanHost[i] = std::tolower(static_cast<unsigned char>(cleanHost[i]));
	}

	for (size_t i = 0; i < _configsREF.size(); ++i) {
		if (_configsREF[i] && _configsREF[i]->hasServerBlockName(cleanHost)) {
			return _configsREF[i];
		}
	}

	return _configsREF.empty() ? NULL : _configsREF[0];
}

// --- Constructors / Destructor ---

ServerBlock::ServerBlock() : _configsREF(), _tcpListener() {}

ServerBlock::ServerBlock(const Config* singleConfig)
	: _configsREF(), _tcpListener() {
	_configsREF.push_back(singleConfig);
}

ServerBlock::~ServerBlock() {}
