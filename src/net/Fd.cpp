/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fd.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:40:44 by elkanega          #+#    #+#             */
/*   Updated: 2026/06/03 15:29:14 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/Fd.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "utils/Logger.hpp"

void Fd::reset(int newFd) {
	if (_fd == newFd) {
		return;
	}
	if (_fd != -1) {
		if (close(_fd) < 0) {
			Logger::logError("Error closing file descriptor during reset.");
		}
		_fd = -1;
	}
	_fd = newFd;
	if (_fd != -1) {
		int flags = fcntl(_fd, F_GETFL, 0);
		if (flags == -1) {
			Logger::logError("Error getting file descriptor flags.");
			close(_fd);
			_fd = -1;
			return;
		}
		if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			Logger::logError("Error setting file descriptor flag");
			close(_fd);
			_fd = -1;
		}
	}
}

Fd::Fd() : _fd(-1) {}

Fd::Fd(int fd) : _fd(-1) {
	reset(fd);
}

Fd::~Fd() {
	reset(-1);
}
