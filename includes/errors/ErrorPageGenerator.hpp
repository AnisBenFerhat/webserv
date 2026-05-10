/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPageGenerator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 11:58:26 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/10 18:10:10 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGEGENERATOR_HPP
#define ERRORPAGEGENERATOR_HPP

#include "http/HttpStatus.hpp"
#include "http/HttpResponse.hpp"
#include "config/ServerBlock.hpp"
#include <string>

/**
 * @brief Finds the best error page available, with a guaranteed internal
 * backup.
 **/
class ErrorPageGenerator {
	public:
		ErrorPageGenerator(const ServerBlock& ServerBlock);
		ErrorPageGenerator(const ErrorPageGenerator& other);
		ErrorPageGenerator& operator=(const ErrorPageGenerator& other);
		~ErrorPageGenerator();

		/**
		 * @brief Builds a complete error response using a fallback
		 * (Config > Disk > Internal).
		 *
		 * @param status The HTTP error code to handle.
		 * @return A fully populated HttpResponse ready to be sent.
		 */
		HttpResponse createResponse(HttpStatus status) const;

	private:
		const ServerBlock& _serverBlock;
		HttpResponse	   _tryCustomPage(HttpStatus status) const;
		HttpResponse	   _tryDefaultFile(HttpStatus status) const;
		HttpResponse	   _generateFallback(HttpStatus status) const;
		std::string		   _readFile(const std::string& path) const;
		HttpResponse	   _buildResponse(HttpStatus		 status,
										  const std::string& body) const;
};

#endif
