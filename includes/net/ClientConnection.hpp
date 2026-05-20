/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:17:23 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/20 15:41:07 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <vector>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "net/Poller.hpp"
#include "net/TcpListener.hpp"
#include "utils/RefCounter.hpp"

class EventLoop;

enum ConnectionState {
	InitialState,
	ReadingRequest,
	Processing,
	WritingResponse,
	KeepAliveWait,
	Closing
};	///< @brief Enum buffers state for safety check.

/**
 * @brief Centralizes the data received and sent by and at the client.
 *
 * It consists of storing the socket from which the client contacted the server
 * to responds it back, keeping a state of the treatment of the request related
 * to the two buffers, one for storing the incoming client request and one to
 * generate the response back.
 */
class ClientConnection : public RefCounter {
	public:
		ClientConnection();
		ClientConnection(int socketFd);
		ClientConnection(const ClientConnection& other);
		ClientConnection& operator=(const ClientConnection& other);
		~ClientConnection();

		/**
		 * @brief Treats data when available to write in the buffer.
		 *
		 * Manages the read of the data sent by the client, parse its headers to
		 * verify request conformity before choosing if a response will be sent
		 * back to turn off the Poller on this socket so it switch to POLLOUT.
		 *
		 * @param Poller address to send the signal.
		 */
		void handleRead(Poller& poller);
		void handleWrite(
			Poller& poller);  ///< @brief When socket is ready to send.

		bool isTimedOut() const;  ///< @brief Timeout state boolean checker.

	private:
		TcpListener		  _tcpListener;	 ///< @brief Storing socket infos.
		std::vector<char> _readBuffer;	 ///< @brief Storing the request.
		std::vector<char> _writeBuffer;	 ///< @brief Storing the response.

		HttpRequest		_request;	///< @brief Decomposed form of the request.
		HttpResponse	_response;	///< @brief Decomposed form of the response.
		ConnectionState _status;	///< @brief Actual state of the request.
};

#endif
