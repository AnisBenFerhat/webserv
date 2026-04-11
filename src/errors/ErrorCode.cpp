/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 22:11:37 by aben-fer          #+#    #+#             */
/*   Updated: 2026/04/05 23:50:35 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/ErrorCode.hpp"

std::string getErrorDescription(ErrorCode code) {
	switch (code) {
		case ERR_NONE:
			return "No Error";
		case ERR_CONFIG_FILE_NOT_FOUND:
			return "Configuration file not found";
		case ERR_CONFIG_PARSE_FAILED:
			return "Failed to parse configuration file";
		case ERR_SOCKET_CREATE_FAILED:
			return "Failed to create socket";
		case ERR_SOCKET_BIND_FAILED:
			return "Failed to bind socket to port";
		case ERR_POLL_FAILED:
			return "Poll multiplexing failed";
		default:
			return "Unknown internal error";
	}
}
