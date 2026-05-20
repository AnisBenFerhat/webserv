/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:31:10 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/15 18:51:52 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Convertor.hpp"

#include <sstream>

const std::string Convertor::_uline = "\033[1;4m";
const std::string Convertor::_reset = "\033[0m";

std::string Convertor::intToStr(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return _uline + ss.str() + _reset;
}

std::string Convertor::uIntToStr(unsigned int nbr) {
	std::stringstream ss;
	ss << nbr;
	return _uline + ss.str() + _reset;
}
