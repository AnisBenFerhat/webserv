/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-fer <aben-fer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 15:03:23 by aben-fer          #+#    #+#             */
/*   Updated: 2026/05/04 16:09:10 by aben-fer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/LocationBlock.hpp"

LocationBlock::LocationBlock()
	: _path(""), _root(""), _index(""), _autoindex(false) {}

LocationBlock::LocationBlock(const LocationBlock& other) {
	*this = other;
}

LocationBlock& LocationBlock::operator=(const LocationBlock& other) {
	if (this != &other) {
		_path	   = other._path;
		_root	   = other._root;
		_index	   = other._index;
		_methods   = other._methods;
		_autoindex = other._autoindex;
	}
	return *this;
}

LocationBlock::~LocationBlock() {}

// Getters
const std::string& LocationBlock::getPath() const {
	return _path;
}

const std::string& LocationBlock::getRoot() const {
	return _root;
}

const std::string& LocationBlock::getIndex() const {
	return _index;
}

const std::vector<std::string>& LocationBlock::getMethods() const {
	return _methods;
}

bool LocationBlock::getAutoindex() const {
	return _autoindex;
}

// Setters
void LocationBlock::setPath(const std::string& path) {
	_path = path;
}

void LocationBlock::setRoot(const std::string& root) {
	_root = root;
}

void LocationBlock::setIndex(const std::string& index) {
	_index = index;
}

void LocationBlock::addMethod(const std::string& method) {
	_methods.push_back(method);
}

void LocationBlock::setAutoindex(bool state) {
	_autoindex = state;
}
