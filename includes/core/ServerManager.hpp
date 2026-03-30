/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 22:56:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/03/30 23:15:39 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <string>

class ServerManager {
	public:
		ServerManager(const std::string& configPath);
		~ServerManager();
		void run();

	private:
		std::string _configPath;
};

#endif
