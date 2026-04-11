/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 23:00:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:29 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/ServerManager.hpp"
#include "utils/Logger.hpp"
#include <cstdlib>
#include <exception>

int main(int argc, char** argv) {
	if (argc > 2) {
		Logger::logError("Usage: ./webserv [config_file]");
		return EXIT_FAILURE;
	}

	try {
		std::string configPath = (argc == 2) ? argv[1] : "conf/default.conf";

		ServerManager manager(configPath);
		manager.run();

	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
