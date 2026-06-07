/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Convertor.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 15:41:49 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/07 12:53:02 by aben-fer         ###   ########.fr       */
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
		 * @brief Convert one or multiple events into a strint literal
		 *
		 * @param The short events to cast
		 * @return std::string of the events input.
		 * */
		static std::string eventsToStr(short events);

		/**
		 * @brief Convert an int into a string literal.
		 *
		 * @param The int number to cast.
		 * @return std::string of the integer input.
		 **/
		static std::string intToStr(int nbr);

		/**
		 * @brief Convert an unsigned int into a string literal.
		 *
		 * @param The unsigned int number to cast.
		 * @return std::string of the integer input.
		 **/
		static std::string uIntToStr(unsigned int nbr);

		/** @brief Returns a lowercase copy of the input string.
		 * Used for case-insensitive HTTP header comparison.
		 * @param str Input string to convert.
		 * @return A lowercase copy of the input string.
		 */
		static std::string toLowerCase(std::string str);

	private:
		Convertor();
		Convertor(const Convertor& src);
		Convertor(const std::string content);
		Convertor& operator=(const Convertor& other);
		~Convertor();
};

#endif
