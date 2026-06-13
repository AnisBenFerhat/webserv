/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Checker.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 19:13:08 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/08 19:20:41 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_HPP
#define CHECKER_HPP

#include <string>

class Checker {
	public:
		static bool isValidIntegerRange(const std::string& str, int min,
										int max, int& outValue);
};

#endif
