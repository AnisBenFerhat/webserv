/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 14:08:13 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/06 14:21:29 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "http/HttpRequest.hpp"
#include <string>
#include <map>
#include <sys/types.h>

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
		 * @return int The read-end File Descriptor of the output pipe, or -1 on
		 * error.
		 **/
		int launchCgiProcess(const HttpRequest& request,
							 const std::string& scriptPath);

		/**
		 * @brief Returns the PID of the new child process.
		 * @return pid_t The child's PID.
		 */
		pid_t getPid() const;

	private:
		std::map<std::string, std::string> _env;
		pid_t							   _pid;

		void   _initEnv(const HttpRequest& request,
						const std::string& scriptPath);
		char** _exportEnv() const;
		void   _freeEnv(char** envp) const;
};

#endif
