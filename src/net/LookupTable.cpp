/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LookupTable.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:14:29 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/15 15:40:05 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/LookupTable.hpp"
#include "net/ClientConnection.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Data Cleaners ---

void LookupTable::removeFd(int socketFd) {
	Logger::logInfo("Removing FD: " + Convertor::intToStr(socketFd));
	_fdToClient.erase(socketFd);
	_fdToCgiPipe.erase(socketFd);
	_fdToServerBlk.erase(socketFd);
}

void LookupTable::clearTable() {
	if (_fdToClient.size() == 0 && _fdToCgiPipe.size() == 0
		&& _fdToServerBlk.size() == 0) {
		return;
	}
	Logger::logInfo("Clearing LookupTable");
	for (std::map<int, ClientConnection*>::const_iterator it = _fdToClient.begin();
		it != _fdToClient.end(); ++it) {
		delete it->second;
	}
	_fdToClient.clear();
	_fdToCgiPipe.clear();
	_fdToServerBlk.clear();
}

// --- Constructors / Destructor

LookupTable::LookupTable() {}

LookupTable::~LookupTable() {
	clearTable();
}
