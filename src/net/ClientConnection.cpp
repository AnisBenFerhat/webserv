/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:15:27 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/05 16:01:32 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/ClientConnection.hpp"

#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sstream>

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"
#include "config/Config.hpp"
#include "errors/ErrorPageGenerator.hpp"
#include "http/Dispatcher.hpp"
#include "http/HttpRequestParser.hpp"
#include "http/RequestRouter.hpp"

static HttpResponse generateErrorResponse(HttpStatus	status,
										  const Config& config) {
	ErrorPageGenerator generator(config);
	return generator.createResponse(status);
}

void ClientConnection::handleRead() {
	if (_status == Closing)
		return;

	if (!_receiveToBuffer())
		return;

	_processHttpRequest();
}

bool ClientConnection::_receiveToBuffer() {
	char tmpBuffer[1024];

	while (true) {
		ssize_t bytesRead =
			recv(_fd->getRawFd(), tmpBuffer, sizeof(tmpBuffer), 0);

		if (bytesRead < 0) {
			int logErrno = errno;
			if (logErrno == EAGAIN || logErrno == EWOULDBLOCK)
				break;
			Logger::logWarning(
				"Read error on FD: " + Convertor::intToStr(_fd->getRawFd()) +
				" - " + std::string(strerror(logErrno)));
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return false;
		}
		if (bytesRead == 0) {
			Logger::logInfo("Client closed connection on FD: " +
							Convertor::intToStr(_fd->getRawFd()));
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return false;
		}
		_readBuffer.insert(_readBuffer.end(), tmpBuffer, tmpBuffer + bytesRead);
		_status = ReadingRequest;
	}
	return true;
}

void ClientConnection::_processHttpRequest() {
	while(!_readBuffer.empty()) {
		HttpRequest					   request;
		std::size_t					   bytesParsed = 0;
		HttpRequestParser::ParseResult result =
			HttpRequestParser::parse(_readBuffer, request, bytesParsed);

		if (result == HttpRequestParser::INCOMPLETE) {
			Logger::logInfo(
				"Partial request on FD: " + Convertor::intToStr(_fd->getRawFd()) +
				" — waiting for more data");
			break;
		}

		if (result == HttpRequestParser::ERROR) {
			Logger::logWarning("Malformed HTTP request on FD: " +
							Convertor::intToStr(_fd->getRawFd()));
			_sendBadRequest();
			_readBuffer.clear();
			return;
		}

		if (!_serverBlk) {
			Logger::logError("No ServerBlock attached to FD: " +
							Convertor::intToStr(_fd->getRawFd()));
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return;
		}

		const std::string& hostHeader = request.getHeader("Host");
		const Config*	   config	  = _serverBlk->getConfigForHost(hostHeader);

		if (!config) {
			Logger::logWarning("No config found for host: [" + hostHeader + "]");
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return;
		}

		const LocationBlock* location =
			RequestRouter::matchLocation(*config, request);

		HttpResponse response;

		if (!location) {
			Logger::logWarning("No location matched URI: [" + request.getPath() +
							"]");
			response = generateErrorResponse(HTTP_404_NOT_FOUND, *config);
		} else {
			response = Dispatcher::dispatch(request, *location, *config);
		}

		std::string serialized = response.serialize();
		_writeBuffer.insert(_writeBuffer.end(), serialized.begin(),
							serialized.end());

		_readBuffer.erase(_readBuffer.begin(), _readBuffer.begin() + bytesParsed);
		if (request.getHeader("Connection") == "close" ||
			response.getHeader("Connection") == "close") {
			_status = Closing;
		} else {
			_status = KeepAliveWait;
		}
	}
	if (!_writeBuffer.empty()) {
		if (_status != Closing) {
			_status = WritingResponse;
		}
		_poller->setEvents(_fd->getRawFd(), POLLOUT);
	}
}

void ClientConnection::_sendBadRequest() {
	std::string badRequest =
		"HTTP/1.1 400 Bad Request\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 50\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html><body><h1>400 Bad Request</h1></body></html>";
	_writeBuffer.insert(_writeBuffer.end(), badRequest.begin(),
						badRequest.end());
	_status = WritingResponse;
	_poller->setEvents(_fd->getRawFd(), POLLOUT);
}

void ClientConnection::handleWrite() {
	if (_writeBuffer.empty()) {
		_poller->setEvents(_fd->getRawFd(), POLLIN);
		return;
	}

	bool yieldToPoll = false;

	while (_writeOffset < _writeBuffer.size()) {
		std::size_t remaining = _writeBuffer.size() - _writeOffset;
		std::size_t toSend	  = remaining < 65536 ? remaining : 65536;

		int bytesSent = send(_fd->getRawFd(), &_writeBuffer[_writeOffset], toSend, 0);
		if (bytesSent < 0) {
			int logErrno = errno;
			if (logErrno == EAGAIN || logErrno == EWOULDBLOCK) {
				_writeBuffer.erase(_writeBuffer.begin(),
								   _writeBuffer.begin() + _writeOffset);
				_writeOffset = 0;
				yieldToPoll = true;
				break;
			}
			Logger::logWarning("Failed while writing into socket FD: " +
							   Convertor::intToStr(_fd->getRawFd()) + " - " +
							   std::string(strerror(logErrno)));
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return;
		}
		if (bytesSent == 0) {
			Logger::logWarning("No bytes sent on FD: " +
							   Convertor::intToStr(_fd->getRawFd()));
			_status = Closing;
			_poller->removeClient(_fd->getRawFd());
			return;
		}
		_writeOffset += static_cast<std::size_t>(bytesSent);
		if (static_cast<std::size_t>(bytesSent) < toSend) {
			yieldToPoll = true;
			break;
		}
		Logger::logInfo("Successfully sent packet into socket FD: " +
						Convertor::intToStr(_fd->getRawFd()));
	}
	if (yieldToPoll) {
		_writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + _writeOffset);
		_writeOffset = 0;
		_poller->setEvents(_fd->getRawFd(), POLLOUT);
	}
	else if (_writeOffset >= _writeBuffer.size()) {
		_writeBuffer.clear();
		_writeOffset = 0;
		if (_status == KeepAliveWait) {
			_status = InitialState;
			_poller->setEvents(_fd->getRawFd(), POLLIN);
			Logger::logInfo("Response fully sent on FD: " +
							Convertor::intToStr(_fd->getRawFd()));
		} else {
			Logger::logInfo("Response fully sent on FD: " +
							Convertor::intToStr(_fd->getRawFd()) +
							" - Closing connection.");
			_poller->removeClient(_fd->getRawFd());
		}
	}
}

bool ClientConnection::isTimedOut() const {
	return (_status == Closing);
}

// Constructors / Destructor

ClientConnection::ClientConnection()
	: RefCounter(),
	  _fd(NULL),
	  _serverBlk(NULL),
	  _poller(NULL),
	  _status(InitialState),
	  _writeOffset(0) {}

ClientConnection::ClientConnection(Fd* fd, const ServerBlock* serverBlk,
								   Poller* poller)
	: RefCounter(),
	  _fd(fd),
	  _serverBlk(serverBlk),
	  _poller(poller),
	  _status(InitialState),
	  _writeOffset(0) {
	if (!_fd)
		Logger::logError("ClientConnection: null Fd pointer");
	if (!_serverBlk)
		Logger::logError("ClientConnection: null ServerBlock pointer");
	if (!_poller)
		Logger::logError("ClientConnection: null Poller pointer");
}

ClientConnection::~ClientConnection() {
	delete _fd;
	_fd = NULL;
}
