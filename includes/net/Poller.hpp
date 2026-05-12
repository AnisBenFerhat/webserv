/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poller.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:55:28 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/12 15:09:58 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLER_HPP
# define POLLER_HPP

#include <sys/poll.h>
#include <vector>

/**
 * @brief Class that wraps the system function poll().
 * Poller maintains an internal vector of pollfd structures.
 * It provides an interface to add, remove, and update monitoring events.
 */
class Poller {
	public:
		Poller();
		Poller(const Poller& other);
		Poller& operator=(const Poller& other);
		~Poller();

		/**
		 * @brief Waits for activity.
		 * @param timeout Time to wait in milliseconds.
		 * @return The number of file descriptors with events, -1 on error, or 0 on timeout.
		 */
		int  pollEvents(int timeout);

		/**
		 * @brief Getter for file desriptors that have pending activity.
		 * @note Returns file descriptors only where revents is not zero after pollEvents() call.
		 * @return Vector of active fd integers.
		 */
		std::vector<int> getFds() const;

		/**
		 * @brief This function updates the event mask for a specific file descriptor.
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

	private:
		std::vector<struct pollfd> _fds;
};

#endif