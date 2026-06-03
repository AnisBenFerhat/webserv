/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 17:11:46 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 13:20:41 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/ServerBlock.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"
#include <map>

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
				// Clean up allocated blocks so far on failure to avoid leaks
				for (size_t j = 0; j < serverBlocks.size(); ++j)
					delete serverBlocks[j];
				delete newServerBlock;
				return std::vector<ServerBlock*>();
			}

			portToIdx[port] = serverBlocks.size();
			// 2. Push back the pointer. No copies are made, no destructors are
			// fired!
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
	for (size_t i = 0; i < _configsREF.size(); ++i) {
		if (_configsREF[i]->hasServerBlockName(hostname)) {
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
