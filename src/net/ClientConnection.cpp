/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:15:27 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/26 17:05:37 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/ClientConnection.hpp"

#include <unistd.h>

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Methods ---

void ClientConnection::handleRead() {
	char tmpBuffer[1024];
	int	 bytesRead =
		recv(_tcpListener.getSocket(), tmpBuffer, sizeof(tmpBuffer), 0);

	if (bytesRead <= 0) {
		if (bytesRead == 0) {
			Logger::logInfo("No bytes read / Client closed connection on FD: " +
							Convertor::intToStr(_tcpListener.getSocket()));
		} else {
			Logger::logWarning("Error reading from socket FD: " +
							   Convertor::intToStr(_tcpListener.getSocket()));
		}

		_poller->removeClient(_tcpListener.getSocket());
		return;
	}

	Logger::logInfo(Convertor::intToStr(bytesRead) +
					" bytes read in socket FD: " +
					Convertor::intToStr(_tcpListener.getSocket()));
	_readBuffer.insert(_readBuffer.end(), tmpBuffer, tmpBuffer + bytesRead);

	// --- MOCK RESPONSE FOR TESTING ---
	std::string mockResponse =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 26\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Test part 1, part 2 is -> just there !";

	_writeBuffer.insert(_writeBuffer.end(), mockResponse.begin(),
						mockResponse.end());
	// --- MOCK RESPONSE FOR TESTING ---

	/* Parsing condition to add there
	 if (parseHttpRequest() == PARSE_SUCCESS) {*/
	_poller->setEvents(_tcpListener.getSocket(), POLLOUT);
	/*} else {
		_poller->setEvents(_tcpListener.getSocket(), POLLIN);
	}*/
}

void ClientConnection::handleWrite() {
	if (_writeBuffer.empty()) {
		_poller->setEvents(_tcpListener.getSocket(), POLLIN);
		return;
	}

	int bytesSent = send(_tcpListener.getSocket(), &_writeBuffer[0],
						 _writeBuffer.size(), 0);

	if (bytesSent < 0) {
		Logger::logWarning("Failed while writing into socket FD: " +
						   Convertor::intToStr(_tcpListener.getSocket()));
		return;
	}
	Logger::logInfo("Successfully sent packet into socket FD: " +
					Convertor::intToStr(_tcpListener.getSocket()));

	_writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + bytesSent);

	if (_writeBuffer.empty()) {
		if (_status == KeepAliveWait) {
			_poller->setEvents(_tcpListener.getSocket(), POLLIN);
		} else {
			_poller->removeClient(_tcpListener.getSocket());
			return;
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

ClientConnection::ClientConnection(int socketFd, struct sockaddr_in address,
								   const ServerBlock* serverBlk, Poller* poller)
	: RefCounter(),
	  _tcpListener(socketFd, address),
	  _poller(poller),
	  _serverBlk(serverBlk),
	  _status(InitialState) {}

ClientConnection::ClientConnection(const ClientConnection& other)
	: RefCounter(other),
	  _tcpListener(other._tcpListener),
	  _readBuffer(other._readBuffer),
	  _writeBuffer(other._writeBuffer),
	  _serverBlk(other._serverBlk),
	  _status(other._status) {}

ClientConnection& ClientConnection::operator=(const ClientConnection& other) {
	if (this != &other) {
		RefCounter::operator=(other);
		_tcpListener = other._tcpListener;
		_readBuffer	 = other._readBuffer;
		_writeBuffer = other._writeBuffer;
		_serverBlk	 = other._serverBlk;
		_status		 = other._status;
	}
	return *this;
}

ClientConnection::~ClientConnection() {}
