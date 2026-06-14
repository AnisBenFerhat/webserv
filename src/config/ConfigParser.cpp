/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 15:36:35 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/13 18:46:11 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/ConfigParser.hpp"

#include <unistd.h>

#include <sstream>

#include "errors/Exceptions.hpp"
#include "utils/Checker.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Parser Methods ---

std::vector<Config> ConfigParser::parseConfig(
    const std::string &configFilePath) {
        std::vector<Config> serverConfigs;
        ConfigParser parser;

        if (parser._openConfigFile(configFilePath) != ERR_NONE ||
            parser._run(serverConfigs) != ERR_NONE ||
            parser._networksChecker(serverConfigs) != ERR_NONE) {
                serverConfigs.clear();
                Logger::logInfo(
                    "Server Configurations cleared due to a parsing error");
        }
        return serverConfigs;
}

ErrorCode ConfigParser::_run(std::vector<Config> &serverConfigs) {
        try {
                for (std::string line; std::getline(_configStream, line);
                     ++_lineCounter) {
                        std::vector<std::string> tokens = _tokenizer(line);

                        if (tokens.empty()) {
                                continue;
                        }
                        if (_validateTokens(tokens, "http {")) {
                                if (_parsingState == GLOBAL_CONTEXT) {
                                        _parsingState = HTTP_CONTEXT;
                                        _sendLog("HTTP block found", 0);
                                } else {
                                        throw std::runtime_error(
                                            "HTTP block found while being in a "
                                            "non global context");
                                }
                        }

                        else if (_validateTokens(tokens, "server {")) {
                                if (_parsingState == HTTP_CONTEXT) {
                                        Config newConfig;
                                        if (_parseServerBlock(newConfig) !=
                                            ERR_NONE) {
                                                return ERR_CONFIG_PARSE_FAILED;
                                        }
                                        serverConfigs.push_back(newConfig);
                                } else {
                                        throw std::runtime_error(
                                            "Server block found while being in "
                                            "a non HTTP context");
                                }
                        }

                        else if (tokens.size() == 1 && tokens.at(0) == "}") {
                                if (_parsingState == HTTP_CONTEXT) {
                                        _lineCounter++;
                                        _sendLog(
                                            "HTTP block parsed successfully",
                                            1);
                                        _parsingState = GLOBAL_CONTEXT;
                                } else {
                                        throw std::runtime_error(
                                            "Closing bracket while being in a "
                                            "non HTTP context "
                                            "found");
                                }
                        } else {
                                throw std::runtime_error(
                                    "Unexpected line detected");
                        }
                }
                if (_parsingState != GLOBAL_CONTEXT) {
                        throw std::runtime_error(
                            "Missing one or more closing brace '}'");
                }
        } catch (const std::runtime_error &e) {
                _sendLog(e.what(), 3);
                _configStream.close();
                Logger::logInfo("Configuration file closed");
                return ERR_CONFIG_PARSE_FAILED;
        }
        _configStream.close();
        Logger::logInfo("Configuration file closed");
        return ERR_NONE;
}

ErrorCode ConfigParser::_parseServerBlock(Config &currentServer) {
        _parsingState = SERVER_CONTEXT;

        _sendLog("Server block found", 0);
        for (std::string line; std::getline(_configStream, line);
             ++_lineCounter) {
                std::vector<std::string> tokens = _tokenizer(line);

                if (tokens.empty()) {
                        continue;
                }

                if (tokens.at(0) == "}") {
                        break;
                }

                if (tokens.size() < 2) {
                        throw std::runtime_error(
                            "Single token line detected [" + tokens.at(0) +
                            "]");
                }
                _dispatchServerDirective(currentServer, tokens);
        }

        if (currentServer.isLocationsEmpty() &&
            currentServer.isDefaultRootEmpty()) {
                LocationBlock defaultLocation;
                defaultLocation.addMethod("GET");
                currentServer.addLocationBlock(defaultLocation);
                _sendLog(
                    "No location blocks found — created default '/' location "
                    "with a GET method",
                    2);
        }
        _lineCounter++;
        _sendLog("Server configuration added", 1);
        _parsingState = HTTP_CONTEXT;
        return ERR_NONE;
}

void ConfigParser::_dispatchServerDirective(
    Config &currentServer, const std::vector<std::string> &tokens) {
        const std::string &key = tokens.at(0);
        const std::string &value = tokens.at(1);

        if (tokens.back() == "{" && key == "location" && tokens.size() == 3) {
                _parseLocationBlock(currentServer, value);
        } else if (tokens.back() == ";") {
                if (key == "server_name" && tokens.size() > 2) {
                        _parseServerBlockName(currentServer, tokens);
                } else if (key == "error_page" && tokens.size() > 3) {
                        _parseErrorPage(currentServer, tokens);
                } else if (tokens.size() == 3) {
                        if (key == "root") {
                                currentServer.setDefaultRoot(value);
                                _sendLog("Default Root set to [" + value + "]",
                                         2);
                        } else if (key == "index") {
                                currentServer.setDefaultIndex(value);
                                _sendLog("Default Index set to [" + value + "]",
                                         2);
                        } else if (key == "listen") {
                                _parseListenDirective(currentServer, value);
                        } else if (key == "client_max_body_size") {
                                _parseMaxBodySize(currentServer, value);
                        } else {
                                throw std::runtime_error(
                                    "Unknown server directive: [" + key + "]");
                        }
                }
        } else {
                throw std::runtime_error("Unknown server directive: [" + key +
                                         "]");
        }
}

void ConfigParser::_parseLocationBlock(Config &currentServer,
                                       const std::string &locationPath) {
        _parsingState = LOCATION_CONTEXT;
        _sendLog("Location block found", 0);

        LocationBlock location;
        std::istringstream iss(locationPath);
        std::string parsedPath;

        iss >> parsedPath;
        if (parsedPath.empty() || parsedPath[0] != '/') {
                throw std::runtime_error("Invalid location path [" +
                                         locationPath + "] detected");
        }
        location.setPath(parsedPath);
        _sendLog("Path set to [" + parsedPath + "]", 2);
        for (std::string line; std::getline(_configStream, line);
             ++_lineCounter) {
                std::vector<std::string> tokens = _tokenizer(line);

                if (tokens.empty()) {
                        continue;
                }

                if (tokens.at(0) == "}") {
                        break;
                }

                if (tokens.size() < 3) {
                        throw std::runtime_error(
                            "Incomplete directive or missing semicolon");
                }
                if (tokens.back() != ";") {
                        throw std::runtime_error(
                            "Directive line must end with a semicolon ';'");
                }

                const std::string key = tokens.at(0);
                const std::string value = tokens.at(1);

                if (key != "allow_methods" && tokens.size() != 3) {
                        throw std::runtime_error(
                            "Unexpected extra arguments for directive [" + key +
                            "]");
                }

                if (key == "root") {
                        location.setRoot(value);
                        _sendLog("Root set to [" + value + "]", 2);
                } else if (key == "index") {
                        location.setIndex(value);
                        _sendLog("Index set to [" + value + "]", 2);
                } else if (key == "autoindex") {
                        if (value == "off" || value == "on") {
                                location.setAutoindex(value == "on");
                                _sendLog("AutoIndex set to [" + value + "]", 2);
                        } else {
                                throw std::runtime_error(
                                    "Incorrect autoindex input [" + value +
                                    "]");
                        }
                } else if (key == "upload_dir") {
                        location.setUploadDir(value);
                        _sendLog("Upload directory set to [" + value + "]", 2);
                } else if (key == "cgi_extension") {
                        location.setCgiExtension(value);
                        _sendLog("CgiExtension set to [" + value + "]", 2);
                } else if (key == "cgi_interpreter") {
                        if (value.empty()) {
                                throw std::runtime_error(
                                    "Missing value for cgi_interpreter");
                        }

                        if (access(value.c_str(), F_OK | X_OK) != 0) {
                                throw std::runtime_error(
                                    "Cgi interpreter: [" + value +
                                    "] does not exist or is not executable");
                        }
                        location.setCgiInterpreter(value);
                        _sendLog("CgiInterpreter set to [" + value + "]", 2);
                } else if (key == "allow_methods") {
                        for (size_t i = 1; i < tokens.size() - 1; i++) {
                                if (tokens.at(i) == "GET" ||
                                    tokens.at(i) == "POST" ||
                                    tokens.at(i) == "DELETE") {
                                        location.addMethod(tokens.at(i));
                                        _sendLog("Allowed method added [" +
                                                     tokens.at(i) + "]",
                                                 2);
                                } else {
                                        throw std::runtime_error(
                                            "Invalid allowed method [" +
                                            tokens.at(i) + "]");
                                }
                        }
                } else {
                        throw std::runtime_error(
                            "Unknown location directive: [" + key + "]");
                }
        }
        if (location.getRoot().empty()) {
                location.setRoot(currentServer.getDefaultRoot());
                _sendLog("Root empty, set by default to [" +
                             location.getRoot() + "]",
                         2);
        }
        if (location.getIndex().empty()) {
                location.setIndex(currentServer.getDefaultIndex());
                _sendLog("Index non defined, set by default to [" +
                             location.getIndex() + "]",
                         2);
        }

        bool hasExtension = !location.getCgiExtension().empty();
        bool hasInterpreter = !location.getCgiInterpreter().empty();

        if (hasExtension && !hasInterpreter) {
                throw std::runtime_error(
                    "Missing cgi_interpreter directive for cgi_extension [" +
                    location.getCgiExtension() + "]");
        } else if (!hasExtension && hasInterpreter) {
                throw std::runtime_error(
                    "Missing cgi_extension directive for cgi_interpreter [" +
                    location.getCgiInterpreter() + "]");
        }
        currentServer.addLocationBlock(location);
        _lineCounter++;
        _sendLog("Location block added: [" + location.getPath() + "]", 1);
        _parsingState = SERVER_CONTEXT;
}

ConfigParser::ConfigParser() : _lineCounter(1), _parsingState(GLOBAL_CONTEXT) {}
