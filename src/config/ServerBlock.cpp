/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 17:11:46 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/04 15:42:48 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/ServerBlock.hpp"

ServerBlock::ServerBlock()
	: _port(8080),
	  _host("0.0.0.0"),
	  _clientMaxBodySize(DEFAULT_MAX_BODY_SIZE) {}

ServerBlock::ServerBlock(const ServerBlock& other) {
	*this = other;
}

ServerBlock& ServerBlock::operator=(const ServerBlock& other) {
	if (this != &other) {
		_port			   = other._port;
		_host			   = other._host;
		_serverNames	   = other._serverNames;
		_errorPages		   = other._errorPages;
		_clientMaxBodySize = other._clientMaxBodySize;
		_locations		   = other._locations;
	}
	return *this;
}

ServerBlock::~ServerBlock() {}

// Getters
int ServerBlock::getPort() const {
	return _port;
}

const std::string& ServerBlock::getHost() const {
	return _host;
}

const std::vector<std::string>& ServerBlock::getServerNames() const {
	return _serverNames;
}

const std::map<int, std::string>& ServerBlock::getErrorPages() const {
	return _errorPages;
}

size_t ServerBlock::getClientMaxBodySize() const {
	return _clientMaxBodySize;
}

const std::vector<LocationBlock>& ServerBlock::getLocations() const {
	return _locations;
}

// Setters
void ServerBlock::setPort(int port) {
	_port = port;
}

void ServerBlock::setHost(const std::string& host) {
	_host = host;
}

void ServerBlock::addServerName(const std::string& name) {
	_serverNames.push_back(name);
}

void ServerBlock::addErrorPage(int code, const std::string& path) {
	_errorPages[code] = path;
}

void ServerBlock::setClientMaxBodySize(size_t size) {
	_clientMaxBodySize = size;
}

void ServerBlock::addLocation(const LocationBlock& location) {
	_locations.push_back(location);
}
