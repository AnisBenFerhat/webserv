/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:36 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/12 14:03:05 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"

// --- Boolean ---

bool Config::hasServerBlockName(const std::string& name) const {
	for (size_t i = 0; i < _serverNames.size(); ++i) {
		if (_serverNames[i] == name) {
			return true;
		}
	}
	return false;
}

// --- Constructors / Destructor

Config::Config()
	: _clientMaxBodySize(DEFAULT_MAX_BODY_SIZE),
	  _defaultIndex(""),
	  _defaultRoot(""),
	  _host("0.0.0.0"),
	  _port(80) {}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		_clientMaxBodySize = other._clientMaxBodySize;
		_defaultIndex	   = other._defaultIndex;
		_defaultRoot	   = other._defaultRoot;
		_errorPages		   = other._errorPages;
		_host			   = other._host;
		_locations		   = other._locations;
		_port			   = other._port;
		_serverNames	   = other._serverNames;
	}
	return *this;
}

Config::~Config() {}
