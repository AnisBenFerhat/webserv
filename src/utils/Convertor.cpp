/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:31:10 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/25 17:37:52 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Convertor.hpp"

#include <poll.h>

#include <sstream>

std::string Convertor::eventsToStr(short events) {
	if (events == 0) {
		return "NONE";
	}

	std::string str;

	if (events & POLLIN) str += "POLLIN ";
	if (events & POLLOUT) str += "POLLOUT ";
	if (events & POLLHUP) str += "POLLHUP ";
	if (events & POLLERR) str += "POLLERR ";
	if (events & POLLNVAL) str += "POLLNVAL ";
	if (events & POLLPRI) str += "POLLPRI ";

	if (!str.empty() && str[str.size() - 1] == ' ') {
		str.erase(str.size() - 1);
	}
	return str;
}

std::string Convertor::intToStr(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}

std::string Convertor::uIntToStr(unsigned int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}
