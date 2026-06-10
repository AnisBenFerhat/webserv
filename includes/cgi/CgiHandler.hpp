/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 14:08:13 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/10 11:17:05 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <sys/types.h>

#include <map>
#include <string>

#include "http/HttpRequest.hpp"
#include "net/Fd.hpp"

/**
 * @brief Manage CGI execution by preparing the environment and forking
 * processes.
 *
 * It deals with the creation of pipes and the execution of the script,
 * returning a file descriptor for non-blocking communication with the web
 * server.
 **/
class CgiHandler {
	public:
		CgiHandler();
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);
		~CgiHandler();

		/**
		 * @brief Forks a new process to execute the CGI script.
		 *
		 * @param request The processed HTTP request.
		 * @param scriptPath The absolute or relative path to the script.
		 * @param interpreter Path to the interpreter binary (e.g.,
		 * /usr/bin/python3).
		 * @return int The read-end File Descriptor of the output pipe, or -1 on
		 * error.
		 **/
		int launchCgiProcess(const HttpRequest& request,
							 const std::string& scriptPath,
							 const std::string& interpreter);

		/**
		 * @brief Returns the PID of the new child process.
		 * @return pid_t The child's PID.
		 */
		pid_t getPid() const { return _pid; }

		/**
		 * @brief Returns read end of CGI stdout pipe
		 * @return pointer to stdout Fd, or NULL if process not launched
		 */
		Fd*	  getStdout() const { return _outFd; }

		/**
		 * @brief Returns write end of CGI stdin pipe
		 * @return pointer to stdin Fd or NULL if process not launched
		 * or no stdin
		 */
		Fd*	  getStdin() const { return _inFd; }

	private:
		std::map<std::string, std::string> _env;
		pid_t							   _pid;
		Fd*								   _outFd;
		Fd*								   _inFd;

		void   _initEnv(const HttpRequest& request,
						const std::string& scriptPath);
		char** _exportEnv() const;
		void   _freeEnv(char** envp) const;
};

#endif
