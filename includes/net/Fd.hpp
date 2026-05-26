/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fd.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 14:40:31 by elkanega          #+#    #+#             */
/*   Updated: 2026/05/25 18:05:10 by flebrun          ###   ########.fr       */
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
		 * @brief Constructs Fd wrapper and sets it to non-blocking
		 * @param fd raw int file descriptor
		 */
		Fd(int fd);

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

	private:
		Fd(const Fd& other);
		Fd& operator=(const Fd& other);
		int _fd;
};

#endif
