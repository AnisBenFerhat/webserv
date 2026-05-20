/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 13:10:50 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/20 16:00:04 by flebrun          ###   ########.fr       */
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
		TcpListener& operator=(const TcpListener& other);
		~TcpListener();

		// --- Methods ---

		/**
		 * @brief Allocates an OS socket and binds it to the target port and
		 * address. Creates an AF_INET, SOCK_STREAM socket and binds it
		 * (typically to port 8080).
		 */
		void initTcp();

		/**
		 * @brief Places the bound socket into a passive listening state.
		 * Readies the system kernel backlog to queue incoming client
		 * handshakes.
		 */
		void listenTcp();

		/**
		 * @brief Extracts the first connection request on the queue of pending
		 * connections.
		 * @return The file descriptor of the newly accepted client connection
		 * socket on success, or a negative integer indicating failure.
		 */
		int acceptTcp();

		/**
		 * @brief Explicitly closes the server listening socket and releases its
		 * file descriptor.
		 */
		void closeTcp();

		// --- Getters ---

		/**
		 * @brief Gets the internal network address configuration structure.
		 * @return A constant reference to the sockaddr_in structure.
		 */
		const sockaddr_in& getAddress();

		/**
		 * @brief Gets the file descriptor allocated to the server socket.
		 * @return The integer file descriptor index tracked by the operating
		 * system.
		 */
		int getSocket();

		// --- Setters ---

		/**
		 * @brief Manually overwrites the internal network address structure
		 * configuration.
		 * @param addressToSet The new target sockaddr_in configuration to
		 * enforce.
		 */
		void setAddress(const sockaddr_in& addressToSet);

		/**
		 * @brief Manually overwrites the active tracking socket descriptor
		 * index.
		 * @param socketToSet The target integer file descriptor index to assume
		 * control over.
		 */
		void setSocket(int socketToSet);

	private:
		/// @brief Stores the server's network config profile (IP & Port).
		sockaddr_in _address;
		/// @brief The active file descriptor index given by the OS kernel.
		int _socket;
};

#endif
