/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:36 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/30 15:39:34 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"

Config::Config() {}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		_servers = other._servers;
	}
	return *this;
}

Config::~Config() {}

void Config::addServer(const ServerBlock& server) {
	_servers.push_back(server);
}

const std::vector<ServerBlock>& Config::getServers() const {
	return _servers;
}

size_t Config::getServerCount() const {
	return _servers.size();
}
