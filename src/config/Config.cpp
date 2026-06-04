/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:36 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 16:34:30 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"

#include <fstream>
#include <sstream>

#include "errors/Exceptions.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// Utils

static std::string trim(const std::string& string) {
	size_t start = string.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	size_t end = string.find_last_not_of(" \t\r\n");
	return string.substr(start, end - start + 1);
}

static std::string stripSemicolon(const std::string& string) {
	if (!string.empty() && string[string.size() - 1] == ';')
		return string.substr(0, string.size() - 1);
	return string;
}

static std::string getKey(const std::string& line) {
	std::istringstream iss(line);
	std::string		   key;
	iss >> key;
	return key;
}

static std::string getValue(const std::string& line) {
	std::istringstream iss(line);
	std::string		   key, value;
	iss >> key >> value;
	return stripSemicolon(value);
}

// --- Parser Methods ---

static LocationBlock parseLocationBlock(std::ifstream&	   configFile,
										const std::string& locationPath) {
	LocationBlock location;
	location.setPath(locationPath);

	std::string line;
	while (std::getline(configFile, line)) {
		std::string trimmedLine = trim(line);

		if (trimmedLine == "}")
			break;
		if (trimmedLine.empty() || trimmedLine[0] == '#')
			continue;

		std::string key	  = getKey(trimmedLine);
		std::string value = getValue(trimmedLine);

		if (key == "root") {
			location.setRoot(value);
		} else if (key == "index") {
			location.setIndex(value);
		} else if (key == "autoindex") {
			location.setAutoindex(value == "on");
		} else if (key == "cgi_extension") {
			location.setCgiExtension(value);
		} else if (key == "cgi_interpreter") {
			location.setCgiInterpreter(value);
		} else if (key == "allowed_methods") {
			std::istringstream iss(trimmedLine);
			std::string		   token;
			iss >> token;
			while (iss >> token) {
				token = stripSemicolon(token);
				if (!token.empty())
					location.addMethod(token);
			}
		} else {
			Logger::logWarning("Unknown location directive: [" + key + "]");
		}
	}
	return location;
}

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
			configTmp = Config();
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
	std::string line;
	while (std::getline(configFile, line)) {
		std::string trimmedLine = trim(line);
		if (trimmedLine == "}")
			break;
		if (trimmedLine.empty() || trimmedLine[0] == '#')
			continue;

		std::string key	  = getKey(trimmedLine);
		std::string value = getValue(trimmedLine);

		if (key == "listen") {
			std::istringstream iss(value);
			int				   port;
			if (iss >> port)
				_port = port;
			else
				Logger::logWarning("Invalid port value: [" + value + "]");
		} else if (key == "host") {
			_host = value;
		} else if (key == "root") {
			_defaultRoot = value;
		} else if (key == "index") {
			_defaultIndex = value;
		} else if (key == "server_name") {
			std::istringstream iss(trimmedLine);
			std::string		   token;
			iss >> token;
			while (iss >> token) {
				token = stripSemicolon(token);
				if (!token.empty())
					_serverNames.push_back(token);
			}
		} else if (key == "client_max_body_size") {
			std::istringstream iss(value);
			size_t			   size;
			if (iss >> size)
				_clientMaxBodySize = size;
			else
				Logger::logWarning("Invalid client_max_body_size: [" + value +
								   "]");
		} else if (key == "error_page") {
			std::istringstream iss(trimmedLine);
			std::string		   token;
			std::string		   path;
			int				   code;
			iss >> token;
			if (iss >> code >> path) {
				path			  = stripSemicolon(path);
				_errorPages[code] = path;
			} else {
				Logger::logWarning("Malformed error_page directive: [" +
								   trimmedLine + "]");
			}
		} else if (key == "location") {
			std::istringstream iss(trimmedLine);
			std::string		   token;
			std::string		   locationPath;
			iss >> token >> locationPath;
			if (!locationPath.empty() &&
				locationPath[locationPath.size() - 1] == '{')
				locationPath = locationPath.substr(0, locationPath.size() - 1);
			locationPath = trim(locationPath);
			if (trimmedLine.find('{') == std::string::npos) {
				std::string openCurlyBracket;
				std::getline(configFile, openCurlyBracket);
			}

			LocationBlock location =
				parseLocationBlock(configFile, locationPath);
			if (location.getRoot().empty())
				location.setRoot(_defaultRoot);
			if (location.getIndex().empty())
				location.setIndex(_defaultIndex);
			_locations.push_back(location);
			Logger::logInfo("Location block added: [" + locationPath + "]");
		} else {
			Logger::logWarning("Unknown server directive: [" + key + "]");
		}
	}

	if (_locations.empty() && !_defaultRoot.empty()) {
		LocationBlock defaultLocation;
		defaultLocation.setPath("/");
		defaultLocation.setRoot(_defaultRoot);
		defaultLocation.setIndex(_defaultIndex);
		defaultLocation.addMethod("GET");
		_locations.push_back(defaultLocation);
		Logger::logInfo(
			"No location blocks found — created default '/' location");
	}
	return ERR_NONE;
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
	: _port(8080),
	  _clientMaxBodySize(DEFAULT_MAX_BODY_SIZE),
	  _host(""),
	  _defaultRoot(""),
	  _defaultIndex("") {}

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
		_defaultRoot	   = other._defaultRoot;
		_defaultIndex	   = other._defaultIndex;
	}
	return *this;
}

Config::~Config() {}
