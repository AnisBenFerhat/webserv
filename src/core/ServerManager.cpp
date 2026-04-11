/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 22:59:27 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:27 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/ServerManager.hpp"
#include "utils/Logger.hpp"

ServerManager::ServerManager(const std::string& configPath)
	: _configPath(configPath) {}

ServerManager::~ServerManager() {}

void ServerManager::run() {
	Logger::logInfo("Webserv is running with: " + _configPath);
}
