/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:17:23 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/11 10:37:28 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <vector>
#include <ctime>

#include "config/ServerBlock.hpp"
#include "net/Fd.hpp"
#include "net/Poller.hpp"
#include "net/TcpListener.hpp"
#include "utils/RefCounter.hpp"
#include "cgi/CgiHandler.hpp"

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

		/**
		 * @brief Binds CGI subprocess descriptors and request body to client
		 * connection. Transfers ownership of pipe Fd objects and stores the
		 * request body as CGI stdin write buffer.
		 * @param cgiIn Write end of CGI stdin pipe
		 * @param cgiOut Read end of CGI stdout pipe
		 * @param cgiPid PID of forked CGI subprocess
		 * @param body Raw request body to be written in CGI stdin pipe
		 */
		void setCgiFields(Fd* cgiIn, Fd* cgiOut,
						  pid_t cgiPid,
						  const std::string& body);

		/**
		 * @brief POLLIN activity on CGI stdout pipe
		 */
		void cgiRead(int pipeFd);

		/**
		 * @brief POLLOUT activity on CGI stdin pipe
		 */
		void cgiWrite(int pipeFd);

		/**
		 * @brief returns CGI stdin pipe owned by this connection
		 */
		Fd*  getCgiIn() const { return _cgiIn; };

		/**
		 * @brief returns CGI stdout pipe owned by this connection
		 */
		Fd*  getCgiOut() const { return _cgiOut; };

		/**
		 * @brief Ends the CGI process and queues a 504 response if allowed
		 * execution time was exceeded.
		 * @param current Current time from time(NULL)
		 * @param timeout max time allowed in seconds
		 */
		void cgiTimeout(time_t current, int timeout);

		/**
		 * @return time_t last activity timestamp
		 */
		time_t getLastActive() { return _timeActive; };

		/**
		 * @brief Updates timestamp. Called at every successful I/O.
		 */
		void updateTimestamp() { _timeActive = time(NULL); };

	private:
		ClientConnection(const ClientConnection& other);
		ClientConnection& operator=(const ClientConnection& other);
		bool _receiveToBuffer();  ///< @brief Reads from socket. Returns false
								  ///< if closed/error.
		void _processHttpRequest();	 ///< @brief Parses, routes, and dispatches
									 ///< the request.
		void _sendBadRequest();		 ///< @brief Utility to send a 400 error.
		void _cleanupCgi();

		std::vector<char>  _readBuffer;	  ///< @brief Storing the request.
		std::vector<char>  _writeBuffer;  ///< @brief Storing the response.
		Fd*				   _fd;
		Fd*				   _cgiIn;
		Fd*				   _cgiOut;
		pid_t			   _cgiPid;
		const ServerBlock* _serverBlk;	///< @brief Retrieve packet size infos.
		Poller*			   _poller;	 ///< @brief Sending orders to the Poller.
		ConnectionState	   _status;	 ///< @brief Actual state of the request.
		bool			   _isKeepAlive;
		std::size_t		   _writeOffset;
		std::string		   _cgiBuffer;
		CgiHandler*		   _activeCgi;
		std::string		   _cgiResponse;
		bool			   _cgiProcessing;
		time_t			   _cgiStart;
		const Config*	   _config;
		time_t			   _timeActive;
};

#endif
