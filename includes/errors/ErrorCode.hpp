/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:10:12 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:45 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORCODE_HPP
#define ERRORCODE_HPP

#include <string>

/**
 * @brief Project error codes
 **/

enum ErrorCode {
	ERR_NONE = 0,
	ERR_CONFIG_FILE_NOT_FOUND,
	ERR_CONFIG_PARSE_FAILED,
	ERR_SOCKET_CREATE_FAILED,
	ERR_SOCKET_BIND_FAILED,
	ERR_POLL_FAILED,
	ERR_UNKNOOWN
};

/**
 * @brief Transform an ErrorCode to a readable string.
 * @param code The internal error code.
 * @return A string description of the error.
 */
std::string getErrorDescription(ErrorCode code);

#endif
