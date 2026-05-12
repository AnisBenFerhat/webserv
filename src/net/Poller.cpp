/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:55:34 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/12 17:20:17 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/Poller.hpp"

Poller::Poller() {}

Poller::Poller(const Poller& other) {
	*this = other;
}

Poller& Poller::operator=(const Poller& other) {
	if (this != &other) {
		_fds = other._fds;
	}
	return *this;
}

Poller::~Poller() {}

int		Poller::pollEvents(int timeout) {
	if (_fds.empty()) {
		return 0;
	}
	int result = poll(&_fds[0], static_cast<nfds_t>(_fds.size()), timeout);
	return result;
}

std::vector<int> Poller::getFds() const {
	std::vector<int> ready;
	for (std::size_t i = 0; i < _fds.size(); ++i) {
		if (_fds[i].revents != 0) {
			ready.push_back(_fds[i].fd);
		}
	}
	return ready;
}

void	Poller::setEvents(int fd, short events) {
	for (std::size_t i = 0; i < _fds.size(); ++i) {
		if(_fds[i].fd == fd) {
			_fds[i].events = events;
			_fds[i].revents = 0;
			return;
		}
	}
}

void	Poller::addFd(int fd, short events) {
	struct pollfd NewFd;
	NewFd.fd = fd;
	NewFd.events = events;
	NewFd.revents = 0;
	_fds.push_back(NewFd);
	return;
}

void	Poller::removeFd(int fd) {
	for(std::vector<struct pollfd>::iterator iter = _fds.begin(); iter != _fds.end(); ++iter) {
		if (iter->fd == fd) {
			if (iter != _fds.end() - 1) {
				*iter = _fds.back();
			}
			_fds.pop_back();
			return;
		}
	}
}

