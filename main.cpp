/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 23:00:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/26 17:26:53 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <exception>

#include "core/ServerManager.hpp"
#include "utils/Logger.hpp"

int main(int argc, char** argv) {
	if (argc > 2) {
		Logger::logError("Usage: ./webserv [config_file]");
		return EXIT_FAILURE;
	}

	try {
		std::string configPath = (argc == 2) ? argv[1] : "conf/default.conf";

		ServerManager manager;
		manager.launch(configPath);

	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
