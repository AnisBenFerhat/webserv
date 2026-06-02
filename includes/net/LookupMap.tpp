/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LookupMap.tpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elkanega <elkanega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 16:56:11 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/01 14:35:00 by elkanega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOOKUPMAP_TPP
#define LOOKUPMAP_TPP

#include <utility>

// --- Data Managers methods ---

template <typename ValueType>
void LookupTable::LookupMap<ValueType>::insert(int socketFdToInsert, const ValueType& value) {
    _map.insert(std::make_pair(socketFdToInsert, value));
}

template <typename ValueType>
void LookupTable::LookupMap<ValueType>::erase(int socketFd) {
    _map.erase(socketFd);
}

template <typename ValueType>
void LookupTable::LookupMap<ValueType>::clear() {
    _map.clear();
}

// --- Const Iterators getters methods ---

template <typename ValueType>
typename std::map<int, ValueType>::const_iterator
LookupTable::LookupMap<ValueType>::getIt(int socketFdToSearch) const {
    return _map.find(socketFdToSearch);
}

template <typename ValueType>
typename std::map<int, ValueType>::const_iterator
LookupTable::LookupMap<ValueType>::begin() const {
    return _map.begin();
}

template <typename ValueType>
typename std::map<int, ValueType>::const_iterator
LookupTable::LookupMap<ValueType>::end() const {
    return _map.end();
}

// --- Size Getter ---

template <typename ValueType>
size_t LookupTable::LookupMap<ValueType>::size() const {
	return _map.size();
}

// --- Constructors / Destructor ---

template <typename ValueType>
LookupTable::LookupMap<ValueType>::LookupMap() : _map() {}

template <typename ValueType>
LookupTable::LookupMap<ValueType>::LookupMap(const LookupMap& other) : _map(other._map) {}

template <typename ValueType>
LookupTable::LookupMap<ValueType>&
LookupTable::LookupMap<ValueType>::operator=(const LookupMap& other) {
    if (this != &other) {
        _map = other._map;
    }
    return *this;
}

template <typename ValueType>
LookupTable::LookupMap<ValueType>::~LookupMap() {}

#endif
