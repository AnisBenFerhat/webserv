/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:09:26 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/25 17:33:24 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

class Logger {
	public:
		/**
		 * @brief Logs an informational message to standard ouput.
		 * @param msg The message to display.
		 **/
		static void logInfo(const std::string& msg);

		/**
		 * @brief Logs a warning message to standard ouput.
		 * @param msg The message to display.
		 **/
		static void logWarning(const std::string& msg);

		/**
		 * @brief Logs a critical error message to error output.
		 * @param msg The message to display.
		 **/
		static void logError(const std::string& msg);

		/**
		 * @brief Logs an part announcing message to standard ouput.
		 * @param msg The message to display.
		 **/
		static void logPart(const std::string& msg);

	private:
		// Colors for terminal output
		static const std::string _cyan;
		static const std::string _yellow;
		static const std::string _red;
		static const std::string _reset;

		Logger();
		Logger(const Logger& src);
		Logger& operator=(const Logger& other);
		~Logger();
};

#endif
