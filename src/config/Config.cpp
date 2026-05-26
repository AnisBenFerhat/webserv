/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:36 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/26 13:36:54 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"

#include <fstream>

#include "errors/Exceptions.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Parser Methods ---

std::vector<Config> Config::parseConfig(const std::string& configFilePath) {
	std::ifstream		configFile(configFilePath.c_str());
	std::string			lineBuffer;
	Config				configTmp;
	std::vector<Config> configs;

	if (_openConfigFile(configFile, configFilePath) != ERR_NONE) {
		return configs;
	}

	for (unsigned int i = 0; std::getline(configFile, lineBuffer); ++i) {
		if (lineBuffer == "server {") {
			Logger::logInfo("Server configuration found at line [" +
							Convertor::uIntToStr(i) + "]");
			if (configTmp._parseServerConf(configFile) != ERR_NONE) {
				continue;
			}
			Logger::logInfo("Server configuration added");
			configs.push_back(configTmp);
		}
	}
	configFile.close();
	Logger::logInfo("Configuration file closed");
	return configs;
}

ErrorCode Config::_openConfigFile(std::ifstream&	configFile,
								  const std::string configFilePath) {
	if (!configFile.is_open()) {
		std::string logMsg("Error while opening configuration file [" +
						   configFilePath + "]: ");
		int			errorMessage(errno);

		switch (errorMessage) {
			case ENOENT:
				Logger::logWarning(logMsg + "The file does not exist");
				break;
			case EACCES:
				Logger::logWarning(logMsg + "Permission denied");
				break;
			default:
				Logger::logError(logMsg + "Undefined behavior");
		}
		return (ERR_CONFIG_PARSE_FAILED);
	}
	Logger::logInfo("Configuration file [" + configFilePath + "] opened");
	return (ERR_NONE);
}

ErrorCode Config::_parseServerConf(std::ifstream& configFile) {
	ErrorCode status = ERR_NONE;

	(void)configFile;
	switch (status) {
		case ERR_NONE:
			break;
		case ERR_CONFIG_PARSE_FAILED:
			Logger::logWarning(getErrorDescription(status));
			break;
		case ERR_UNKNOWN:
		default:
			Logger::logError(getErrorDescription(status));
	}
	return status;
}

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
	: _port(8080), _clientMaxBodySize(DEFAULT_MAX_BODY_SIZE), _host("") {}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		_port			   = other._port;  ///< @brief [1024-65535] valid range.
		_clientMaxBodySize = other._clientMaxBodySize;
		_errorPages		   = other._errorPages;
		_host			   = other._host;
		_locations		   = other._locations;
		_serverNames	   = other._serverNames;
	}
	return *this;
}

Config::~Config() {}
