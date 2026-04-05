/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:11:33 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:38 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/Exceptions.hpp"

// Webserv Exception
WebservException::WebservException(const std::string& message)
	: _message(message) {}

WebservException::~WebservException() throw() {}

const char* WebservException::what() const throw() {
	return _message.c_str();
}

// Config Exception
ConfigException::ConfigException(const std::string& message)
	: WebservException(message) {}

ConfigException::~ConfigException() throw() {}

// Runtime Exception
RuntimeException::RuntimeException(const std::string& message)
	: WebservException(message) {}

RuntimeException::~RuntimeException() throw() {}
