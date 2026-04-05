/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:11:31 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:48 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

/**
 * @brief Base class for all Webserv exceptions.
 **/

class WebservException : public std::exception {
	public:
		WebservException(const std::string& message);
		virtual ~WebservException() throw();
		virtual const char* what() const throw();

	protected:
		std::string _message;
};

/**
 * @brief Exception for config syntax or logical errors.
 */
class ConfigException : public WebservException {
	public:
		ConfigException(const std::string& message);
		virtual ~ConfigException() throw();
};

/**
 * @brief Exception for runtime system or network failures.
 */
class RuntimeException : public WebservException {
	public:
		RuntimeException(const std::string& message);
		virtual ~RuntimeException() throw();
};

#endif
