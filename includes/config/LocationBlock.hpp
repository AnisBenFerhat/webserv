/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 15:03:06 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/03 15:41:49 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <string>
#include <vector>

/**
 * @brief Stores configuration rules for a specific URL prefix (route).
 **/
class LocationBlock {
	public:
		LocationBlock();
		LocationBlock(const LocationBlock& other);
		LocationBlock& operator=(const LocationBlock& other);
		~LocationBlock();

		// Getters
		const std::string&				getPath() const;
		const std::string&				getRoot() const;
		const std::string&				getIndex() const;
		const std::vector<std::string>& getMethods() const;
		bool							getAutoindex() const;

		// Setters
		void setPath(const std::string& path);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void addMethod(const std::string& method);
		void setAutoindex(bool state);

	private:
		std::string				 _path;
		std::string				 _root;
		std::string				 _index;
		std::vector<std::string> _methods;
		bool					 _autoindex;
};

#endif
