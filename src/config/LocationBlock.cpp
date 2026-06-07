/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 15:03:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/06/06 10:41:53 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/LocationBlock.hpp"

LocationBlock::LocationBlock()
	: _path(""),
	  _root(""),
	  _index(""),
	  _autoindex(false),
	  _cgiExtension(""),
	  _cgiInterpreter(""),
	  _uploadDir("") {
}

LocationBlock::LocationBlock(const LocationBlock& other) {
	*this = other;
}

LocationBlock& LocationBlock::operator=(const LocationBlock& other) {
	if (this != &other) {
		_path = other._path;
		_root = other._root;
		_index = other._index;
		_methods = other._methods;
		_autoindex = other._autoindex;
		_cgiExtension = other._cgiExtension;
		_cgiInterpreter = other._cgiInterpreter;
		_uploadDir = other._uploadDir;
	}
	return *this;
}

LocationBlock::~LocationBlock() {
}
