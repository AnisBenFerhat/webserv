/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:12:58 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/20 17:09:18 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/TcpListener.hpp"

#include <memory.h>
#include <unistd.h>

#include <cstring>

void TcpListener::initTcp() {
	_socket					 = socket(AF_INET, SOCK_STREAM, 0);
	_address.sin_family		 = AF_INET;
	_address.sin_port		 = htons(8080);
	_address.sin_addr.s_addr = INADDR_ANY;

	bind(_socket, (struct sockaddr*)&_address, sizeof(_address));
}

void TcpListener::listenTcp() {
	listen(_socket, 5);
}

int TcpListener::acceptTcp() {
	return accept(_socket, 0, 0);
}

void TcpListener::closeTcp() {
	close(_socket);
}

const sockaddr_in& TcpListener::getAddress() {
	return _address;
}

int TcpListener::getSocket() {
	return _socket;
}

void TcpListener::setAddress(const sockaddr_in& addressToSet) {
	_address = addressToSet;
}

void TcpListener::setSocket(int socketToSet) {
	_socket = socketToSet;
}

TcpListener::TcpListener() {}

TcpListener::TcpListener(int existingSocketFd) {
	setSocket(existingSocketFd);
	std::memset(&_address, 0, sizeof(_address));
}

TcpListener::TcpListener(const TcpListener& other)
	: _address(other._address), _socket(other._socket) {}

TcpListener& TcpListener::operator=(const TcpListener& other) {
	if (this != &other) {
		_address = other._address;
		_socket	 = other._socket;
	}
	return (*this);
}

TcpListener::~TcpListener() {}
