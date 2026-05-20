/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RefCounter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 15:06:09 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/17 18:53:43 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/RefCounter.hpp"

void RefCounter::add_ref() {
	++_refCount;
}

void RefCounter::release() {
	--_refCount;
	if (_refCount == 0) {
		delete this;
	}
}

RefCounter::RefCounter() : _refCount(1) {}	// Starts with 1 owner

RefCounter::RefCounter(const RefCounter& other) : _refCount(other._refCount) {}

RefCounter& RefCounter::operator=(const RefCounter& other) {
	if (this != &other) {
		_refCount = other._refCount;
	}
	return (*this);
}

RefCounter::~RefCounter() {}
