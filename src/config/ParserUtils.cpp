/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:21:35 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/13 16:54:20 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include <iostream>
#include <sstream>

#include "config/ConfigParser.hpp"
#include "utils/Checker.hpp"
#include "utils/Logger.hpp"

void ConfigParser::_parseListenDirective(Config&			currentServer,
										 const std::string& value) {
	std::string ipStr	= "";
	std::string portStr = "";

	size_t colonPos = value.find(':');
	if (colonPos != std::string::npos) {
		ipStr	= value.substr(0, colonPos);
		portStr = value.substr(colonPos + 1);
	} else if (value.find('.') != std::string::npos) {
		ipStr	= value;
		portStr = "8080";
	}

	if (ipStr.empty()) {
		ipStr = "0.0.0.0";
	}

	bool   isValidIp = true;
	size_t dotCount	 = 0;

	for (size_t i = 0; i < ipStr.length(); ++i) {
		if (ipStr[i] == '.') {
			dotCount++;
		} else if (!std::isdigit(ipStr[i])) {
			isValidIp = false;
			break;
		}
	}

	if (!isValidIp || dotCount != 3) {
		throw std::runtime_error("Invalid characters or malformed host IP [" +
								 ipStr + "]");
	}

	std::istringstream ipIss(ipStr);
	std::string		   segment;
	while (std::getline(ipIss, segment, '.')) {
		if (segment.empty() || segment.length() > 3) {
			throw std::runtime_error("Malformed IP segment [" + ipStr + "]");
		}
		std::istringstream segIss(segment);
		int				   octet;
		segIss >> octet;
		if (octet < 0 || octet > 255) {
			throw std::runtime_error("IP octet out of range [0-255] in [" +
									 ipStr + "]");
		}
	}

	bool isValidPort = true;
	for (size_t i = 0; i < portStr.length(); ++i) {
		if (!std::isdigit(portStr[i])) {
			isValidPort = false;
			break;
		}
	}

	if (!isValidPort || portStr.empty()) {
		throw std::runtime_error("Invalid non-numeric port value [" + portStr +
								 "]");
	}

	std::istringstream portIss(portStr);
	int				   port;
	if (portIss >> port && port >= 1024 && port <= 65535) {
		currentServer.setHost(ipStr);
		currentServer.setPort(port);
		_sendLog("Host set to [" + ipStr + "]", 2);
		_sendLog("Port set to [" + Convertor::intToStr(port) + "]", 2);
	} else {
		throw std::runtime_error("Invalid port value: [" + value + "]");
	}
}

ErrorCode ConfigParser::_openConfigFile(const std::string& configFilePath) {
	_configStream.open(configFilePath.c_str());

	if (!_configStream.is_open()) {
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

bool ConfigParser::_validateTokens(const std::vector<std::string>& tokens,
								   const std::string&			   pattern) {
	std::vector<std::string> patternTokens;

	std::stringstream ss(pattern);
	std::string		  token;

	while (ss >> token) {
		patternTokens.push_back(token);
	}

	if (tokens.empty() || tokens.size() != patternTokens.size()) {
		return false;
	}

	for (int i = tokens.size() - 1; i >= 0; --i) {
		if (tokens[i] != patternTokens[i]) {
			return false;
		}
	}
	return true;
}

std::vector<std::string> ConfigParser::_tokenizer(const std::string& line) {
	std::vector<std::string> tokens;
	std::string				 cleanLine = line;

	// Erasing commentary elements
	size_t commentPos = cleanLine.find('#');
	if (commentPos != std::string::npos) {
		cleanLine.erase(commentPos);
	}

	// Separating brackets and semicolon if they're stuck to a word
	std::string formattedLine = "";
	for (size_t i = 0; i < cleanLine.length(); ++i) {
		char ch = cleanLine[i];
		if (ch == ';' || ch == '{' || ch == '}') {
			formattedLine += " ";
			formattedLine += ch;
			formattedLine += " ";
		} else {
			formattedLine += ch;
		}
	}

	// Splitting the string into tokens
	std::stringstream ss(formattedLine);
	std::string		  token;

	while (ss >> token) {
		tokens.push_back(token);
	}

	if (tokens.empty()) {
		return tokens;
	}

	const std::string& lastToken = tokens.back();

	if (lastToken == ";" || lastToken == "{" || lastToken == "}") {
		for (size_t i = 0; i < tokens.size() - 1; ++i) {
			if (tokens[i] == ";" || tokens[i] == "{" || tokens[i] == "}") {
				throw std::runtime_error(
					"Syntax error: Unexpected structural token '" + tokens[i] +
					"' before end of line");
			}
		}
	} else {
		throw std::runtime_error(
			"Syntax error: Line must end with a semicolon ';' or a bracket "
			"'{' / '}'");
	}
	return tokens;
}

void ConfigParser::_parseMaxBodySize(Config&			currentServer,
									 const std::string& value) {
	bool	  isValid	 = !value.empty();
	size_t	  i			 = 0;
	long long size		 = 0;
	long long multiplier = 1;

	do {
		if (!isValid) {
			break;
		}

		while (i < value.length() && std::isdigit(value[i])) {
			i++;
		}

		std::string num_part  = value.substr(0, i);
		std::string unit_part = value.substr(i);

		if (num_part.empty()) {
			isValid = false;
			break;
		}

		std::stringstream ss(num_part);
		ss >> size;

		if (!unit_part.empty()) {
			if (unit_part.length() > 1) {
				isValid = false;
				break;
			}

			char unit = std::tolower(unit_part[0]);
			if (unit == 'k') {
				multiplier = 1024;
			} else if (unit == 'm') {
				multiplier = 1024 * 1024;
			} else {
				isValid = false;
				break;
			}
		}

		if (size > DEFAULT_MAX_BODY_SIZE / multiplier) {
			isValid = false;
			break;
		}

	} while (false);

	if (!isValid) {
		throw std::runtime_error("Invalid client_max_body_size: [" + value +
								 "]");
	}

	size_t result = static_cast<size_t>(size * multiplier);
	currentServer.setClientMaxBodySize(result);
	_sendLog(
		"Client Max Body Size set to [" + Convertor::uIntToStr(result) + "]",
		2);
}

void ConfigParser::_parseServerBlockName(
	Config& currentServer, const std::vector<std::string>& tokens) {
	for (size_t i = 1; i < tokens.size() - 1; ++i) {
		std::string nameToken = tokens[i];

		if (nameToken.find(':') != std::string::npos) {
			throw std::runtime_error("Syntax error: server_name [" + nameToken +
									 "] cannot contain a port definition ':'");
		}

		currentServer.addServerBlockName(nameToken);
		_sendLog("Added Server Block Name [" + nameToken + "]", 2);
	}
}

void ConfigParser::_parseErrorPage(Config& currentServer,
								   const std::vector<std::string>& tokens) {
	std::string path = tokens[tokens.size() - 2];

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(
			"Custom error page file does not exist or cannot be read: [" +
			path + "]");
	}

	bool parsedAtLeastOneCode = false;
	for (size_t i = 1; i < tokens.size() - 2; ++i) {
		int code = 0;

		if (Checker::isValidIntegerRange(tokens[i], 300, 599, code)) {
			currentServer.addErrorPage(code, path);
			_sendLog("Added Error Page path [" + path + "] to the code [" +
						 Convertor::intToStr(code) + "]",
					 2);
			parsedAtLeastOneCode = true;
		} else {
			throw std::runtime_error("Invalid error code [" + tokens[i] + "]");
		}
	}

	if (!parsedAtLeastOneCode) {
		throw std::runtime_error("No valid HTTP error codes found");
	}
}

ErrorCode ConfigParser::_networksChecker(const std::vector<Config>& configs) {
	for (size_t i = 0; i < configs.size(); ++i) {
		for (size_t j = i + 1; j < configs.size(); ++j) {
			if (configs[i].getPort() == configs[j].getPort() &&
				configs[i].getHost() == configs[j].getHost()) {
				const std::vector<std::string>& namesI =
					configs[i].getServerBlockName();
				for (size_t k = 0; k < namesI.size(); ++k) {
					if (configs[j].hasServerBlockName(namesI[k])) {
						Logger::logError(
							"Duplicate server binding "
							"found [" +
							configs[i].getHost() + ":" +
							Convertor::uIntToStr(configs[i].getPort()) +
							"] with overlapping server_name [" + namesI[k] +
							"]");
						return ERR_CONFIG_PARSE_FAILED;
					}
				}
			}
		}
	}
	return ERR_NONE;
}

void ConfigParser::_sendLog(const std::string& msg, int printOpt) {
	static const std::string TREE_LOOKUP[3][3] = {
		{". ", "`--- ", ". "},						// GLOBAL|HTPP_CONTEXT
		{"|--- ", "|  `--- ", "|  |--- "},			// SERVER_CONTEXT
		{"|  |--- ", "|  |  `--- ", "|  |  |--- "}	// LOCATION_CONTEXT
	};

	int row = 0;
	if (_parsingState == SERVER_CONTEXT) {
		row = 1;
	}
	if (_parsingState == LOCATION_CONTEXT) {
		row = 2;
	}

	std::string asciiTree =
		(printOpt >= 0 && printOpt < 3) ? TREE_LOOKUP[row][printOpt] : "";

	std::string line = Convertor::uIntToStr(_lineCounter + _parsingState - 2);
	bool		includeLine = (printOpt == 0 || printOpt == 3);
	std::string fullMsg =
		asciiTree + msg + (includeLine ? " at line [" + line + "]" : "");

	if (printOpt == 3) {
		_log("|");
		Logger::logWarning("`--- " + fullMsg);
		_log("");
	} else {
		_log(fullMsg);
	}

	// 3. Handle trailing spacing/context lines cleanly
	if (printOpt == 1) {
		if (_parsingState == HTTP_CONTEXT) {
			_log("");
		}
		if (_parsingState == SERVER_CONTEXT) {
			_log("|");
		}
		if (_parsingState == LOCATION_CONTEXT) {
			_log("|  |");
		}
	}
}

void ConfigParser::_log(const std::string& log) {
	std::cout << "       " << log << std::endl;
}
