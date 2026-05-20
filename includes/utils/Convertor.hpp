/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:41:49 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/15 18:42:58 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONVERTOR_HPP
#define CONVERTOR_HPP

#include <string>

/**
 * @class Convertor that converts data type into another.
 * @brief For std::string format outputs, the text is underlined.
 */
class Convertor {
	public:
		/**
		 * @brief Tools that takes an int as a parameter to convert it into a
		 * string.
		 * @param The int number to cast.
		 * @placement At the end of a log.
		 * @return std::string of the integer input with a dot after it.
		 **/
		static std::string intToStr(int nbr);
		/**
		 * @brief Tools that takes an unsigned int as a parameter to convert it
		 * into a string.
		 * @param The unsigned int number to cast.
		 * @placement At the end of a log.
		 * @return std::string of the integer input with a dot after it.
		 **/
		static std::string uIntToStr(unsigned int nbr);

	private:
		// Underline code for terminal output clarity
		static const std::string _uline;
		static const std::string _reset;

		Convertor();
		Convertor(const Convertor& src);
		Convertor(const std::string content);
		Convertor& operator=(const Convertor& other);
		~Convertor();
};

#endif
