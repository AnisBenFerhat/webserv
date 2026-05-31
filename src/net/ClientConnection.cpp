/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:15:27 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/31 17:09:08 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/ClientConnection.hpp"

#include <unistd.h>
#include <cerrno>

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Methods ---

void ClientConnection::handleRead() {
	if (_status == Closing)
		return;
	char tmpBuffer[1024];

	while(true) {
		ssize_t	 bytesRead =
			recv(_fd->getRawFd(), tmpBuffer, sizeof(tmpBuffer), 0);
		if (bytesRead > 0) {
			_readBuffer.insert(_readBuffer.end(), tmpBuffer, tmpBuffer + bytesRead);
			_status = ReadingRequest;
			continue;
		}
		if (bytesRead == 0) {
			Logger::logInfo("No bytes read / Client closed connection on FD: " +
							Convertor::intToStr(_fd->getRawFd()));
			_poller->removeClient(_fd->getRawFd());
			_status = Closing;
			return;
		}
		if (bytesRead < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			else {
				Logger::logWarning("Read error on FD: " +
								  Convertor::intToStr(_fd->getRawFd()));
				_poller->removeClient(_fd->getRawFd());
				return;
			}
		}
		_status = Closing;
		return;
	}

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
	// TODO Parsing condition to add there and remove mock response by a real one
	// if (parseHttpRequest() == PARSE_SUCCESS) {
		_status = WritingResponse;
		_poller->setEvents(_fd->getRawFd(), POLLOUT);
	//} else {
	//	_poller->setEvents(_fd.getRawFd(), POLLIN);
	//}
}

void ClientConnection::handleWrite() {
	if (_status == Closing || _writeBuffer.empty()) {
		_poller->setEvents(_fd->getRawFd(), POLLIN);
		return;
	}
	std::size_t total = 0;
	while (total < _writeBuffer.size()) {
		std::size_t remaining = _writeBuffer.size() - total;
		std::size_t toSend = remaining < 4096 ? remaining : 4096;
		//&_writeBuffer[0] on an empty vector is undefined behavior
		int bytesSent = send(_fd->getRawFd(), &_writeBuffer[total],
							toSend, 0);
		if (bytesSent < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				_writeBuffer.erase(_writeBuffer.begin(),
								_writeBuffer.begin() + total);
			}
			Logger::logWarning("Failed while writing into socket FD: " +
							Convertor::intToStr(_fd->getRawFd()));
			_poller->removeClient(_fd->getRawFd());
			return;
		}
		if (bytesSent == 0) {
			Logger::logWarning("No bytes sent: " +
							Convertor::intToStr(_fd->getRawFd()));
			break;
		}
		total += static_cast<std::size_t>(bytesSent);
	}
	Logger::logInfo("Successfully sent packet into socket FD: " +
					Convertor::intToStr(_fd->getRawFd()));

	_writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + total);
	if (_writeBuffer.empty()) {
		if (_status == KeepAliveWait) {
			_poller->setEvents(_fd->getRawFd(), POLLIN | POLLOUT);
		} else {
			_poller->removeClient(_fd->getRawFd());
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
	: RefCounter(),
	  _fd(),
	  _serverBlk(NULL),
	  _poller(NULL),
	  _status(InitialState) {}


ClientConnection::ClientConnection(Fd* fd, const ServerBlock* serverBlk,
								Poller* poller)
	: RefCounter(),
	  _fd(fd),
	  _serverBlk(serverBlk),
	  _poller(poller),
	  _status(InitialState) {
	if (!_fd) {
		Logger::logError("ClientConnection: null Fd pointer");
	}
	if (!_serverBlk) {
		Logger::logError("ClientConnection: null ServerBlock pointer");
	}
	if (!_poller) {
		Logger::logError("ClientConnection: null Poller pointer");
	}
}

ClientConnection::~ClientConnection() {
	delete(_fd);
	_fd = NULL;
}
