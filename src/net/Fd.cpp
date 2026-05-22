/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fd.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:40:44 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/22 15:34:11 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/Fd.hpp"
#include "utils/Logger.hpp"
#include <unistd.h>
#include <fcntl.h>

Fd::Fd() : _fd(-1) {}

Fd::Fd(int fd) : _fd(fd) {
	if (_fd != -1) {
		int flags = fcntl(_fd, F_GETFL, 0);
		if (flags == -1) {
			flags = 0;
		}
		if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			Logger::logError("Error setting file descriptor flag");
			close(_fd);
			_fd = -1;
		}
	}
}

Fd::~Fd() {
	if (_fd != -1) {
		close(_fd);
	}
}

int	Fd::getRawFd() const {
	return(_fd);
}
