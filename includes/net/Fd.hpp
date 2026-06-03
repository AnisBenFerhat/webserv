/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fd.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:40:31 by elkanega          #+#    #+#             */
/*   Updated: 2026/06/03 15:39:05 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FD_HPP
#define FD_HPP

/**
 * @brief A resource management class for file descriptors and event-driven
 * resources to ensure safety and non-blocking compatability. The constructor
 * will take an int as a parameter and set the flag to non-blocking.
 */
class Fd {
	public:
		Fd();

		/**
		 * @brief Constructs Fd wrapper and sets it to non-blocking. Must be
		 * explicit to prevent implicit conversions.
		 * @param fd raw int file descriptor
		 */
		explicit Fd(int fd);

		/**
		 * @brief Destructor that automatically closes the file descriptor
		 */
		~Fd();


		/**
		 * @brief Get raw int file descriptor for Poller to pass to poll()
		 */
		int getRawFd() const {
			return _fd;
		}

		/**
		 * @brief Resets file descriptor, safely closing any existing handle
		 * and configuring the new one. If `newFd` matches the current
		 * descriptor, this method returns immediately. Otherwise, any active
		 * socket is cleanly closed. If `newFd` is a valid descriptor, it is
		 * assigned and configured with the O_NONBLOCK flag. If config fails,
		 * the socket is safely closed and internal state rolls back to -1.
		 * @param newFd file descriptor to use, or -1 to release the
		 * current resource without assigning a new one.
		 */
		void reset(int newFd);

	private:
		Fd(const Fd& other);
		Fd& operator=(const Fd& other);
		int _fd;
};

#endif
