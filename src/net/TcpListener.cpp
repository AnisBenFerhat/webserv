/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:12:58 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/26 16:26:35 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/TcpListener.hpp"

#include <memory.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

void TcpListener::initTcp(int port) {
	std::memset(&_address, 0, sizeof(_address));

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
		Logger::logError("Failed to create socket: " +
						 std::string(std::strerror(errno)));
		return;
	}
	_isOwner = true;
	Logger::logInfo("Socket created successfully with FD [" +
					Convertor::intToStr(_socket) + "]");

	_address.sin_family		 = AF_INET;
	_address.sin_port		 = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		Logger::logError("setsockopt(SO_REUSEADDR) failed: " +
						 std::string(std::strerror(errno)));
		closeTcp();
		return;
	}

	if (bindTcp() < 0) {
		Logger::logError("bind() has failed, closing the TcpListener");
		closeTcp();
		return;
	}

	if (listenTcp() < 0) {
		Logger::logError("listen() has failed, closing the TcpListener");
		closeTcp();
		return;
	}

	Logger::logInfo("Server successfully initialized on port " +
					Convertor::intToStr(port));
}

int TcpListener::bindTcp() {
	return bind(_socket, (struct sockaddr*)&_address, sizeof(_address));
}

int TcpListener::listenTcp() {
	return listen(_socket, 5);
}

void TcpListener::closeTcp() {
	if (_socket >= 0) {
		close(_socket);
	}
	_socket = -1;
}

// --- Constructors / Destructor

TcpListener::TcpListener() : _socket(-1), _isOwner(false) {
	std::memset(&_address, 0, sizeof(_address));
}

TcpListener::TcpListener(int existingSocketFd)
	: _socket(existingSocketFd), _isOwner(false) {
	std::memset(&_address, 0, sizeof(_address));
}

TcpListener::TcpListener(int existingSocketFd, struct sockaddr_in address)
	: _address(address), _socket(existingSocketFd), _isOwner(false) {}

TcpListener::TcpListener(const TcpListener& other)
	: _address(other._address), _socket(other._socket), _isOwner(false) {}

TcpListener& TcpListener::operator=(const TcpListener& other) {
	if (this != &other) {
		// To prevent loss of an already existing socket
		if (_socket >= 0 && _isOwner) {
			closeTcp();
		}
		_address = other._address;
		_socket	 = other._socket;
		_isOwner = false;
	}
	return (*this);
}

TcpListener::~TcpListener() {
	if (_socket >= 0 && _isOwner) {
		closeTcp();
	}
}
