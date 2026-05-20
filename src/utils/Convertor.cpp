/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:31:10 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/20 17:09:54 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Convertor.hpp"

#include <sstream>

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
