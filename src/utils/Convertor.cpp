/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:31:10 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/12 14:12:41 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Convertor.hpp"

#include <poll.h>

#include <sstream>
#include <string>

bool isValidIntegerRange(const std::string& str, int min, int max,
						 int& outValue) {
	if (str.empty()) return false;

	// 1. Strict digit check (ensures no trailing garbage like "8080abc" passes)
	for (size_t i = 0; i < str.length(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
			return false;
		}
	}

	// 2. Convert string to integer safely using stringstream
	std::stringstream ss(str);
	long long		  tempValue;  // Use a larger type to safely catch overflows

	if (!(ss >> tempValue)) {
		return false;  // Conversion failed
	}

	// 3. Range Verification
	if (tempValue < min || tempValue > max) {
		return false;  // Out of bounds
	}

	outValue = static_cast<int>(tempValue);
	return true;
}

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

std::string Convertor::toLowerCase(std::string str) {
	for (size_t i = 0; i < str.length(); ++i) {
		str[i] = std::tolower(str[i]);
	}
	return str;
}
