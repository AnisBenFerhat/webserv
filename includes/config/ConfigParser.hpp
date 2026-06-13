/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 14:12:38 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/13 18:42:21 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <fstream>
#include <string>
#include <vector>

#include "config/Config.hpp"
#include "errors/ErrorCode.hpp"

/**
 * @brief Manages server configuration data parsing.
 *
 * This class populates Config object to return them in a vector container form.
 * Each Config object that it creates is made from the parsing of the
 * configuration file provided via command-line arguments that is composed of
 * `nginx-style` blocks.
 */
class ConfigParser {
	public:
		/**
		 * @brief Orchestrate the parsing and returns server blocks.
		 *
		 * @param configFilePath The path to the `.conf` file to be parsed.
		 * @return A vector of populated Config objects.
		 */
		static std::vector<Config> parseConfig(
			const std::string& configFilePath);

	private:
		enum State {
			GLOBAL_CONTEXT,
			HTTP_CONTEXT,
			SERVER_CONTEXT,
			LOCATION_CONTEXT
		};	///< @brief Describe the actual parsing context

		ConfigParser();	 ///< @brief Forces usage of static entry point.

		// --- Internal Parsing Framework ---

		/**
		 * @brief Dispatch the configurations parsing between http and server
		 * blocks.
		 *
		 * Checks braces related errors and manages the creation of server
		 * blocks.
		 * @return ErrorCode representing the success or syntax error within the
		 * block.
		 */
		ErrorCode _run(std::vector<Config>& serverConfigs);

		/**
		 * @brief Parses a single 'server {}' block and populates a Config
		 * object.
		 *
		 * @param configFile Server Block positioned open file stream.
		 * @return ErrorCode representing the success or syntax error within the
		 * block.
		 */
		ErrorCode _parseServerBlock(Config& currentServer);

		/**
		 * @brief Recognizes the type of directive read to call the
		 * corresponding element parser.
		 *
		 * @param The full line parsed from the config.
		 */
		void _dispatchServerDirective(Config&						  server,
									  const std::vector<std::string>& tokens);

		/**
		 * @brief Parses a full location block to population LocationBlock
		 * object
		 *
		 * By checking the value of the key, values are being parsed and added
		 * to the LocationBlock directly added to the actual Config being
		 * populated.
		 */
		void _parseLocationBlock(Config&			currentServer,
								 const std::string& locationPath);

		ErrorCode _networksChecker(const std::vector<Config>& configs);
		/**
		 * @brief Internal helper to open and validate the configuration
		 * file stream.
		 *
		 * @param configFile A reference to the ifstream to be opened.
		 * @param configFilePath Printing path to the file purpose.
		 * @return ErrorCode For success or failure result.
		 */
		ErrorCode _openConfigFile(const std::string& configFilePath);

		// --- Utils & Tokenizers ---

		/**
		 * @brief Transforms a string into single words vector.
		 */
		std::vector<std::string> _tokenizer(const std::string& line);

		/**
		 * @brief Centralizes the log for easier formatting
		 *
		 * Member function that print the message passed through parameter while
		 * adding ascii tree prefix by checking _parsingStatus enum and adding
		 * if needed the _lineCounter precision after the message.
		 */
		void _sendLog(const std::string& msg, int printOpt);

		/**
		 * @brief Custom version of the logInfo to prevent printing [INFO]
		 */
		static void _log(const std::string& log);

		/**
		 * @brief Transforms a pattern string into single words vector to
		 * compare it word by word to another vector of tokens.
		 */
		bool _validateTokens(const std::vector<std::string>& tokens,
							 const std::string&				 pattern);

		// --- Key related parser setters

		/**
		 * @brief Checks for size_t overflow and the only digit accepted values
		 * ('b'/'B', 'm'/'M')
		 */
		void _parseMaxBodySize(Config& currentServer, const std::string& value);

		/**
		 * @brief Checks for byte overflow, port acceptance and the only digit
		 * accepted values ('.', ':')
		 */
		void _parseListenDirective(Config&			  currentServer,
								   const std::string& value);

		/**
		 * @brief Checks for non alphanumeric characters
		 */
		void _parseServerBlockName(Config& currentServer,
								   const std::vector<std::string>& tokens);

		/**
		 * @brief Checks for in bound error codes numbers and the readability of
		 * the HTML pages
		 */
		void _parseErrorPage(Config&						 currentServer,
							 const std::vector<std::string>& tokens);

		// --- Parser Private State Variables ---
		std::ifstream
			   _configStream;  ///< @brief Structure for reading the conf file.
		size_t _lineCounter;  ///< @brief Global value indicating parsing y axe.
		State  _parsingState;  //< @brief Actual context of the parsing.
};

#endif
