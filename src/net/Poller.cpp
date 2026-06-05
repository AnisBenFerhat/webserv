/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:55:34 by elkanega          #+#    #+#             */
/*   Updated: 2026/06/05 15:56:22 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "net/Poller.hpp"

#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "net/ClientConnection.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// --- Specific Methods ---

void Poller::removeClient(int clientFd) {
	std::map<int, ClientConnection*>::const_iterator clientIt =
		_lookupTable.getClientIt(clientFd);
	if (clientIt == _lookupTable.getClientEndIt()) {
		Logger::logWarning("Attempted to remove non-existent client FD: " +
						   Convertor::intToStr(clientFd));
		return;
	}

	ClientConnection* connectionPtr = clientIt->second;

	_lookupTable.removeFd(clientFd);
	removeFd(clientFd);
	delete connectionPtr;

	Logger::logInfo("Client connection on FD [" +
					Convertor::intToStr(clientFd) +
					"] cleanly deallocated and shut down.");
}

int Poller::acceptTcpConnection(int socketFd, struct sockaddr* client_addr,
								socklen_t& client_len) {
	int clientFd = accept(socketFd, client_addr, &client_len);

	if (clientFd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return -1;
		}
		Logger::logWarning("Socket FD [" + Convertor::intToStr(socketFd) +
						   "] failed accepting an entering connection: " +
						   std::string(strerror(errno)));
		return -1;
	}
	Logger::logInfo("Socket FD [" + Convertor::intToStr(socketFd) +
					"] accepted a new connection [" +
					Convertor::intToStr(clientFd) + "]");
	return clientFd;
}

void Poller::acceptNewConnection(int socketFd, const ServerBlock* serverBlk) {
	while (true) {
		struct sockaddr_in clientAddr;
		socklen_t		   clientLen = sizeof(clientAddr);
		std::memset(&clientAddr, 0, sizeof(clientAddr));

		int clientFd = acceptTcpConnection(
			socketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), clientLen);
		if (clientFd < 0) {
			return;
		}

		Fd*	heapFd = new Fd(clientFd);
		if (heapFd->getRawFd() < 0) {
			delete heapFd;
			return;
		}

		ClientConnection* newConnection = new ClientConnection(heapFd,
										serverBlk, this);
		this->addFd(clientFd, POLLIN);
		this->getLookupTable().insertClient(clientFd, newConnection);
	}
}

void Poller::initPoller(const std::vector<ServerBlock*>& serverBlocks) {
	int	listenerFd;

	for (std::vector<ServerBlock*>::const_iterator it = serverBlocks.begin();
		 it != serverBlocks.end(); ++it) {
		listenerFd = (*it)->getSocket();

		if (listenerFd >= 0) {
			_lookupTable.insertServerBlk(listenerFd, *it);
			addFd(listenerFd, POLLIN);
		}
	}
	Logger::logInfo("Total number of active Fds in Poller [" +
					Convertor::intToStr(_lookupTable.getServerBlkSize()) + "]");
}

void Poller::handleFdActivity(int fd, short revents) {
	Logger::logPart("Poller detected activity on FD " +
					Convertor::intToStr(fd));
// Listening Socket - Incoming connection
	std::map<int, const ServerBlock*>::const_iterator ServerBlkIt =
		_lookupTable.getServerBlkIt(fd);
	if (ServerBlkIt != _lookupTable.getServerBlkEndIt()) {
		Logger::logInfo("Found corresponding ServerBlock to FD [" +
						Convertor::intToStr(fd) + "]");
		if (revents & POLLIN) {
			acceptNewConnection(ServerBlkIt->first, ServerBlkIt->second);
		}
		return;
	}
// CGI pipe
	std::map<int, ClientConnection*>::const_iterator CgiPipeIt =
		_lookupTable.getCgiPipeIt(fd);
	if (CgiPipeIt != _lookupTable.getCgiPipeEndIt()) {
		Logger::logInfo("Found corresponding CgiPipe to FD [" +
						Convertor::intToStr(fd) + "]");
		if (revents & POLLIN) {
			CgiPipeIt->second->handleRead();
		}
		return;
	}
//high-level HTTP client connection branch
	std::map<int, ClientConnection*>::const_iterator clientIt =
		_lookupTable.getClientIt(fd);
	if (clientIt != _lookupTable.getClientEndIt()) {
		Logger::logInfo("Found corresponding Client to FD [" +
						Convertor::intToStr(fd) + "]");

		if (revents & (POLLHUP | POLLERR | POLLNVAL)) {
			Logger::logWarning("Client disconnected abruptly on FD [" +
							   Convertor::intToStr(fd) + "]");
			removeClient(fd);
			return;
		}

		if (revents & POLLIN) {
			clientIt->second->handleRead();
		} else if (revents & POLLOUT) {
			clientIt->second->handleWrite();
		}
		return;
	}

	Logger::logError(
		"FATAL SPIN: Activity detected on FD [" + Convertor::intToStr(fd) +
		"] but it matches NO server, CGI, or Client in the lookup tables!");
	std::exit(1);
}

int Poller::pollEvents(int timeout) {
	if (_fds.empty()) {
		return 0;
	}
	int result = poll(&_fds[0], static_cast<nfds_t>(_fds.size()), timeout);
	if (result < 0) {
		int logErrno = errno;
		if (logErrno == EINTR) {
			return 0;
		}
		Logger::logError("poll() failure: " +
						 std::string(strerror(logErrno)));
		return -1;
	}
	return result;
}

void Poller::dispatchActivity() {
	std::vector<struct pollfd> activeFds;
	for (std::size_t i = 0; i < _fds.size(); ++i) {
		if (_fds[i].revents != 0) {
			activeFds.push_back(_fds[i]);
		}
	}

	for (std::size_t i = 0; i < activeFds.size(); ++i) {
		int fd = activeFds[i].fd;
		short revents = activeFds[i].revents;
		if (_lookupTable.getServerBlkIt(fd) == _lookupTable.getServerBlkEndIt() &&
			_lookupTable.getCgiPipeIt(fd) == _lookupTable.getCgiPipeEndIt() &&
			_lookupTable.getClientIt(fd) == _lookupTable.getClientEndIt()) {
			Logger::logInfo("Skipping destroyed FD [" +
							 Convertor::intToStr(fd) + "] during dispatch");
			continue;
		}
		handleFdActivity(fd, revents);
	}
}

// --- Getters / Setters ---

std::vector<int> Poller::getFds() const {
	std::vector<int> ready;
	for (std::size_t i = 0; i < _fds.size(); ++i) {
		if (_fds[i].revents != 0) {
			ready.push_back(_fds[i].fd);
		}
	}
	return ready;
}
//TODO
//Will need to modify events flags when CGI pipes are active
void Poller::setEvents(int fd, short events) {
	if (fd < 0) {
		Logger::logWarning("Invalid file descriptor.");
		return;
	}
	for (std::size_t i = 0; i < _fds.size(); ++i) {
		if (_fds[i].fd == fd) {
			_fds[i].events	= events;
			_fds[i].revents = 0;
			return;
		}
	}
}

void Poller::addFd(int fd, short events) {
	struct pollfd NewFd;
	NewFd.fd	  = fd;
	NewFd.events  = events;
	NewFd.revents = 0;
	_fds.push_back(NewFd);
	Logger::logInfo("Socket added to the Poller [" + Convertor::intToStr(fd) +
					"] (" + Convertor::eventsToStr(events) + ")");
	return;
}

void Poller::removeFd(int fd) {
	for (std::vector<struct pollfd>::iterator iter = _fds.begin();
		 iter != _fds.end(); ++iter) {
		if (iter->fd == fd) {
			if (iter != _fds.end() - 1) {
				*iter = _fds.back();
			}
			Logger::logInfo("Socket removed from the Poller [" +
							Convertor::intToStr(fd) + "] (" +
							Convertor::eventsToStr(iter->events) + ")");
			_fds.pop_back();
			return;
		}
	}
}

void Poller::clearTable() {
	_fds.clear();
	_lookupTable.clearTable();
}

// --- Constructors / Destructor ---

Poller::Poller() {}

Poller::~Poller() {
	clearTable();
}
