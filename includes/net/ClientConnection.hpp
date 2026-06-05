/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:17:23 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/04 13:42:00 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <vector>

#include "config/ServerBlock.hpp"
#include "net/Fd.hpp"
#include "net/Poller.hpp"
#include "net/TcpListener.hpp"
#include "utils/RefCounter.hpp"

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
		ClientConnection(Fd* fd, const ServerBlock* serverBlk, Poller* poller);
		~ClientConnection();

		void handleRead();		  ///< @brief When data is available to write.
		void handleWrite();		  ///< @brief When socket is ready to send.
		bool isTimedOut() const;  ///< @brief Timeout state boolean checker.

	private:
		ClientConnection(const ClientConnection& other);
		ClientConnection& operator=(const ClientConnection& other);
		bool _receiveToBuffer();  ///< @brief Reads from socket. Returns false
								  ///< if closed/error.
		void _processHttpRequest();	 ///< @brief Parses, routes, and dispatches
									 ///< the request.
		void _sendBadRequest();		 ///< @brief Utility to send a 400 error.

		std::vector<char>  _readBuffer;	  ///< @brief Storing the request.
		std::vector<char>  _writeBuffer;  ///< @brief Storing the response.
		Fd*				   _fd;
		const ServerBlock* _serverBlk;	///< @brief Retrieve packet size infos.
		Poller*			   _poller;	 ///< @brief Sending orders to the Poller.
		ConnectionState	   _status;	 ///< @brief Actual state of the request.
		std::size_t		   _writeOffset;
};

#endif
