/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 23:00:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/03/30 23:06:55 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/ServerManager.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
	if (argc > 2) {
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return EXIT_FAILURE;
	}

	std::string configPath = (argc == 2) ? argv[1] : "conf/default.conf";

	ServerManager manager(configPath);
	manager.run();

	return EXIT_SUCCESS;
}
