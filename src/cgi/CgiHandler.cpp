/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 14:08:21 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/03 15:37:53 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi/CgiHandler.hpp"

#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include "utils/Logger.hpp"

CgiHandler::CgiHandler() : _pid(-1) {}

CgiHandler::CgiHandler(const CgiHandler& other) {
	*this = other;
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	if (this != &other) {
		this->_env = other._env;
		this->_pid = other._pid;
	}
	return *this;
}

CgiHandler::~CgiHandler() {}

int CgiHandler::launchCgiProcess(const HttpRequest& request,
								 const std::string& scriptPath,
								 const std::string& interpreter) {
	_initEnv(request, scriptPath);

	int pipeOut[2];
	if (pipe(pipeOut) == -1)
		return -1;

	int pipeIn[2];
	if (pipe(pipeIn) == -1) {
		close(pipeOut[0]);
		close(pipeOut[1]);
		return -1;
	}

	_pid = fork();
	if (_pid == -1) {
		close(pipeOut[0]);
		close(pipeOut[1]);
		close(pipeIn[0]);
		close(pipeIn[1]);
		return -1;
	}

	if (_pid == 0) {
		dup2(pipeOut[1], STDOUT_FILENO);

		close(pipeOut[0]);
		close(pipeOut[1]);

		dup2(pipeIn[0], STDIN_FILENO);
		close(pipeIn[0]);
		close(pipeIn[1]);

		char** envp = _exportEnv();
		char*  args[3];
		args[0] = const_cast<char*>(interpreter.c_str());
		args[1] = const_cast<char*>(scriptPath.c_str());
		args[2] = NULL;

		execve(interpreter.c_str(), args, envp);

		_freeEnv(envp);
		std::exit(1);
	}

	close(pipeOut[1]);
	close(pipeIn[0]);

	const std::string& body = request.getBody();
	if (!body.empty()) {
		ssize_t written = write(pipeIn[1], body.c_str(), body.size());
		if (written == -1 || static_cast<size_t>(written) != body.size()) {
			Logger::logWarning("Incomplete write to CGI stdin for script: [" +
							   scriptPath + "]");
		}
	}
	close(pipeIn[1]);

	return pipeOut[0];
}

void CgiHandler::_initEnv(const HttpRequest& request,
						  const std::string& scriptPath) {
	_env["REQUEST_METHOD"]	  = request.getMethodString();
	_env["SCRIPT_FILENAME"]	  = scriptPath;
	_env["PATH_INFO"]		  = request.getPath();
	_env["CONTENT_TYPE"]	  = request.getHeader("Content-Type");
	_env["CONTENT_LENGTH"]	  = request.getHeader("Content-Length");
	_env["HTTP_COOKIE"]		  = request.getHeader("Cookie");
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"]	  = "HTTP/1.1";

	const std::string& path		= request.getPath();
	size_t			   queryPos = path.find('?');
	if (queryPos != std::string::npos)
		_env["QUERY_STRING"] = path.substr(queryPos + 1);
}

char** CgiHandler::_exportEnv() const {
	char** envp = new char*[_env.size() + 1];
	size_t i	= 0;

	for (std::map<std::string, std::string>::const_iterator it = _env.begin();
		 it != _env.end(); ++it) {
		std::string entry = it->first + "=" + it->second;
		envp[i]			  = new char[entry.length() + 1];
		std::strcpy(envp[i], entry.c_str());
		i++;
	}
	envp[i] = NULL;

	return envp;
}

void CgiHandler::_freeEnv(char** envp) const {
	if (!envp)
		return;
	for (size_t i = 0; envp[i] != NULL; ++i) {
		delete[] envp[i];
	}
	delete[] envp;
}
