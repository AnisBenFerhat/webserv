/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:10:50 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/26 16:40:07 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <netinet/in.h>
#include <sys/socket.h>

/**
 * @brief Class responsible of the listening socket lifecycle.
 *
 * The TcpListener initialize the socket connection by creating and binding it,
 * listen to int and accepts connections before being closed.
 */
class TcpListener {
	public:
		// --- Constructors / Destructor
		TcpListener();
		TcpListener(int existingSocketFd);
		TcpListener(const TcpListener& other);
		TcpListener(int existingSocketFd, struct sockaddr_in address);
		TcpListener& operator=(const TcpListener& other);
		~TcpListener();

		// --- Methods ---

		/**
		 * @brief Allocates an OS socket and binds it to the target port and
		 * address. Creates an AF_INET, SOCK_STREAM socket and binds it
		 * (typically to port 8080).
		 */
		void initTcp(int port);

		/**
		 * @brief Bind the socket FD to it's structure to store events and
		 * revents when the kernel monitors it.
		 */
		int bindTcp();

		/**
		 * @brief Places the bound socket into a passive listening state.
		 * Readies the system kernel backlog to queue incoming client
		 * handshakes.
		 */
		int listenTcp();

		/**
		 * @brief Explicitly closes the server listening socket and releases its
		 * file descriptor.
		 */
		void closeTcp();

		// --- Getters ---

		const sockaddr_in& getAddress() {
			return _address;
		}
		int getSocket() const {
			return _socket;
		}

		// --- Setters ---

		void setAddress(const sockaddr_in& addressToSet) {
			_address = addressToSet;
		}
		void setSocket(int socketToSet) {
			_socket = socketToSet;
		}

	private:
		/// @brief Stores the server's network config profile (IP & Port).
		sockaddr_in _address;
		/// @brief The active file descriptor index given by the OS kernel.
		int _socket;
		/// @brief Track who actually owns the resource if a copy has been made.
		bool _isOwner;
};

#endif
