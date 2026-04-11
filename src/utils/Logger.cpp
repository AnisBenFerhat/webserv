/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:09:50 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:40 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Logger.hpp"
#include <iostream>

const std::string Logger::_cyan	  = "\033[1;36m";
const std::string Logger::_yellow = "\033[1;33m";
const std::string Logger::_red	  = "\033[1;31m";
const std::string Logger::_reset  = "\033[0m";

void Logger::logInfo(const std::string& msg) {
	std::cout << _cyan << "[INFO] " << _reset << msg << std::endl;
}

void Logger::logWarning(const std::string& msg) {
	std::cout << _yellow << "[WARNING] " << _reset << msg << std::endl;
}

void Logger::logError(const std::string& msg) {
	std::cerr << _red << "[ERROR] " << _reset << msg << std::endl;
}
