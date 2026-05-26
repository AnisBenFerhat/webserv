/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fd.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:40:44 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/25 18:04:56 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/Fd.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "utils/Logger.hpp"

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
