/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 22:59:27 by aben-fer          #+#    #+#             */
/*   Updated: 2026/03/30 23:15:36 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/ServerManager.hpp"
#include <iostream>

ServerManager::ServerManager(const std::string& configPath)
	: _configPath(configPath) {}

ServerManager::~ServerManager() {}

void ServerManager::run() {
	std::cout << "Webserv is running with: " << _configPath << std::endl;
}
