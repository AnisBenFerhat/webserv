/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LookupTable.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:14:29 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/25 18:09:30 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/LookupTable.hpp"

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
	Logger::logInfo("Clearing LookupTable");
	_fdToClient.clear();
	_fdToCgiPipe.clear();
	_fdToServerBlk.clear();
}

// --- Constructors / Destructor

LookupTable::LookupTable() {}

LookupTable::LookupTable(const LookupTable& other)
	: _fdToClient(other._fdToClient),
	  _fdToCgiPipe(other._fdToCgiPipe),
	  _fdToServerBlk(other._fdToServerBlk) {}

LookupTable& LookupTable::operator=(const LookupTable& other) {
	if (this != &other) {
		_fdToClient	   = other._fdToClient;
		_fdToCgiPipe   = other._fdToCgiPipe;
		_fdToServerBlk = other._fdToServerBlk;
	}
	return *this;
}

LookupTable::~LookupTable() {}
