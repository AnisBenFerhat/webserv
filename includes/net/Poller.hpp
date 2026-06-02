/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:55:28 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/28 16:14:51 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLER_HPP
#define POLLER_HPP

#include <sys/poll.h>

#include <vector>

#include "net/LookupTable.hpp"

/**
 * @brief Class that wraps the system function poll().
 * Poller maintains an internal vector of pollfd structures.
 * It provides an interface to add, remove, and update monitoring events.
 */
class Poller {
	public:
		// --- Specific Methods ---

		void removeClient(int clientFd);
		/**
		 * @brief Set all the events needed at launch.
		 * @param Container of ServerBlock that contains their respective socket
		 * fd.
		 */
		void initPoller(const std::vector<ServerBlock*>& serverBlocks);

		/**
		 * @brief Waits for activity.
		 * @param timeout Time to wait in milliseconds.
		 * @return The number of file descriptors with events, -1 on error, or 0
		 * on timeout.
		 */
		int pollEvents(int timeout);

		/**
		 * @brief Extracts the first connection request on the queue of pending
		 * connections.
		 * @param The address of the struct sockaddr and it's len to write
		 * inside of the value.
		 * @return The file descriptor of the newly accepted client connection
		 * socket on success, or a negative integer indicating failure.
		 */
		int acceptTcpConnection(int socketFd, struct sockaddr* client_addr,
								socklen_t& client_len);

		void acceptNewConnection(int socketFd, const ServerBlock* serverBlk);

		/**
		 * @brief Determines the response of an FD activity.
		 * @param The socket FD that created activity.
		 *
		 * Retrieves the origin of an FD looking at the maps to make the rights
		 * activity in response to it.
		 */
		void handleFdActivity(int fd, short revents);

		/**
		 * @brief Loops through active FDs to init handleActivity on them.
		 *
		 * HandleActivity() method needs revents param taht is linked to the
		 * struct pollfd. This methods helps to check every pollfd and call
		 * HandleActivity() on them when there is activity detected after a
		 * poll() call.
		 */
		void dispatchActivity();

		// --- Getters / Setters

		LookupTable& getLookupTable() {
			return _lookupTable;
		};
		/**
		 * @brief Getter for file desriptors that have pending activity.
		 * @note Returns file descriptors only where revents is not zero after
		 * pollEvents() call.
		 * @return Vector of active fd integers.
		 */
		std::vector<int> getFds() const;

		/**
		 * @brief This function updates the event mask for a specific file
		 * descriptor.
		 * @param fd File descriptor to modify.
		 * @param events New bitmask (POLLIN/POLLOUT).
		 */
		void setEvents(int fd, short events);

		/**
		 * @brief Add new connection
		 * @param fd File descriptor to add.
		 * @param events The initial bitmask (usually POLLIN).
		 */
		void addFd(int fd, short events);

		/**
		 * @brief Removes the fd and stops monitoring.
		 * @param int File descriptor to be removed
		 */
		void removeFd(int fd);

		/**
		 * @brief Erase every Fd stored in the LookupTable.
		 * */
		void clearTable();

		// --- Constructors / Destructor
		Poller();
		~Poller();

	private:
		Poller(const Poller& other);
		Poller& operator=(const Poller& other);
		std::vector<struct pollfd> _fds;
		LookupTable				   _lookupTable;
};

#endif
