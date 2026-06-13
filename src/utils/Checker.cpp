/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Checker.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 19:09:38 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/13 16:55:56 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Checker.hpp"

#include <sstream>

bool Checker::isValidIntegerRange(const std::string& str, int min, int max,
								  int& outValue) {
	if (str.empty()) return false;

	for (size_t i = 0; i < str.length(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
			return false;
		}
	}

	std::stringstream ss(str);
	long long		  tempValue;

	if (!(ss >> tempValue)) {
		return false;
	}

	if (tempValue < min || tempValue > max) {
		return false;
	}

	outValue = static_cast<int>(tempValue);
	return true;
}
