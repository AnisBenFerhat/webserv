/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 11:42:16 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/30 16:42:15 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <cstddef>
#include "config/ServerBlock.hpp"

/**
 * @brief Root configuration class holding all server blocks.
 **/
class Config {
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		// Getters
		const std::vector<ServerBlock>& getServers() const;
		std::size_t						getServerCount() const;

		// Setters
		void addServer(const ServerBlock& server);

	private:
		std::vector<ServerBlock> _servers;
};

#endif
