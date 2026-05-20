/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:15:27 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/20 15:56:50 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/ClientConnection.hpp"

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Methods ---

void ClientConnection::handleRead(Poller& poller) {
	char tmpBuffer[1024];
	int	 bytesRead =
		recv(_tcpListener.getSocket(), tmpBuffer, sizeof(tmpBuffer), 0);

	if (bytesRead == 0) {
		_status = Closing;
		return;
	} else if (bytesRead < 0) {
		Logger::logWarning("Failed while reading into socket FD: " +
						   Convertor::intToStr(_tcpListener.getSocket()));
		return;
	}

	_readBuffer.insert(_readBuffer.end(), tmpBuffer, tmpBuffer + bytesRead);

	// Parsing condition to add there
	// if (parseHttpRequest() == PARSE_SUCCESS) {
	poller.setEvents(_tcpListener.getSocket(), POLLOUT);
	//}
}

void ClientConnection::handleWrite(Poller& poller) {
	if (_writeBuffer.empty()) {
		return;
	}

	int bytesSent = send(_tcpListener.getSocket(), &_writeBuffer[0],
						 _writeBuffer.size(), 0);

	if (bytesSent < 0) {
		Logger::logWarning("Failed while writing into socket FD: " +
						   Convertor::intToStr(_tcpListener.getSocket()));
		return;
	}

	_writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + bytesSent);

	if (_writeBuffer.empty()) {
		if (_status == KeepAliveWait) {
			poller.setEvents(_tcpListener.getSocket(), POLLIN);
		} else {
			_status = Closing;
		}
	}
}

bool ClientConnection::isTimedOut() const {
	if (_status == Closing) {
		return true;
	}
	return false;
}

// --- Constructors / Destructor

ClientConnection::ClientConnection()
	: RefCounter(), _tcpListener(), _status(InitialState) {}

ClientConnection::ClientConnection(int socketFd)
	: RefCounter(), _tcpListener(socketFd), _status(InitialState) {}

ClientConnection::ClientConnection(const ClientConnection& other)
	: RefCounter(other),
	  _tcpListener(other._tcpListener),
	  _readBuffer(other._readBuffer),
	  _writeBuffer(other._writeBuffer) {}

ClientConnection& ClientConnection::operator=(const ClientConnection& other) {
	if (this != &other) {
		RefCounter::operator=(other);
		_tcpListener = other._tcpListener;
		_readBuffer	 = other._readBuffer;
		_writeBuffer = other._writeBuffer;
	}
	return *this;
}

ClientConnection::~ClientConnection() {}
