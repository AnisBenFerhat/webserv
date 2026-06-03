/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:12:58 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/03 14:34:36 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/TcpListener.hpp"

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

	int rawSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (rawSocket < 0) {
		Logger::logError("Failed to create socket: " +
						 std::string(std::strerror(errno)));
		return;
	}
	_socket.reset(rawSocket);
	if (_socket.getRawFd() == -1) {
		Logger::logError("Failed to configure socket flags.");
		return;
	}

	Logger::logInfo("Socket created successfully with FD [" +
					Convertor::intToStr(_socket.getRawFd()) + "]");

	_address.sin_family		 = AF_INET;
	_address.sin_port		 = htons(port);
	_address.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	if (setsockopt(_socket.getRawFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
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
	return bind(_socket.getRawFd(), reinterpret_cast<struct sockaddr*>(&_address), sizeof(_address));
}

int TcpListener::listenTcp() {
	return listen(_socket.getRawFd(), 128);
}

void TcpListener::closeTcp() {
	_socket.reset(-1);
}

TcpListener::TcpListener() : _socket(-1) {
	std::memset(&_address, 0, sizeof(_address));
}

TcpListener::TcpListener(int existingSocketFd)
	: _socket(existingSocketFd) {
	std::memset(&_address, 0, sizeof(_address));
}

TcpListener::TcpListener(int existingSocketFd, struct sockaddr_in address)
	: _address(address), _socket(existingSocketFd) {}

TcpListener::~TcpListener() {}
