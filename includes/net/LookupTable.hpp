/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LookupTable.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 19:03:16 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/25 18:10:30 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOOKUPTABLE_HPP
#define LOOKUPTABLE_HPP

#include <map>

#include "config/Config.hpp"
#include "config/ServerBlock.hpp"

class ClientConnection;

/**
 * @brief Establish maps connecting socket FD and Config / ServerBlock.
 *
 * LookupTable identifies each socketFd to a ServerBlock or a Config pointer so
 * the Cgi points directly at the right data, resulting in a more efficient and
 * less time consuming result.
 */
class LookupTable {
	private:
		template <typename ValueType>
		class LookupMap {
			private:
				std::map<int, ValueType> _map;

			public:
				LookupMap();
				LookupMap(const LookupMap& other);
				LookupMap& operator=(const LookupMap& other);
				~LookupMap();

				void insert(int socketFdToInsert, const ValueType& value);
				void erase(int socketFd);
				void clear();
				typename std::map<int, ValueType>::const_iterator getIt(
					int socketFdToSearch) const;
				typename std::map<int, ValueType>::const_iterator end() const;
				size_t											  size() const;
		};

	public:
		typedef typename std::map<int, ClientConnection*>::const_iterator
			client_iterator;
		typedef typename std::map<int, const ServerBlock*>::const_iterator
			server_iterator;

		LookupTable();
		LookupTable(const LookupTable& other);
		LookupTable& operator=(const LookupTable& other);
		~LookupTable();

		void removeFd(int socketFd);
		void clearTable();

		// --- Setters ---
		void insertClient(int fd, ClientConnection* clientCon) {
			_fdToClient.insert(fd, clientCon);
		}
		void insertCgiPipe(int fd, ClientConnection* clientCon) {
			_fdToCgiPipe.insert(fd, clientCon);
		}
		void insertServerBlk(int fd, const ServerBlock* serverBlk) {
			_fdToServerBlk.insert(fd, serverBlk);
		}

		// --- Iterators Getters ---
		client_iterator getClientIt(int fd) const {
			return _fdToClient.getIt(fd);
		}
		client_iterator getCgiPipeIt(int fd) const {
			return _fdToCgiPipe.getIt(fd);
		}
		server_iterator getServerBlkIt(int fd) const {
			return _fdToServerBlk.getIt(fd);
		}

		client_iterator getClientEndIt() const {
			return _fdToClient.end();
		}
		client_iterator getCgiPipeEndIt() const {
			return _fdToCgiPipe.end();
		}
		server_iterator getServerBlkEndIt() const {
			return _fdToServerBlk.end();
		}

		// --- Size Getters ---
		int getClientSize() const {
			return _fdToClient.size();
		}
		int getCgiPipeSize() const {
			return _fdToCgiPipe.size();
		}
		int getServerBlkSize() const {
			return _fdToServerBlk.size();
		}

	private:
		LookupMap<ClientConnection*>  _fdToClient;
		LookupMap<ClientConnection*>  _fdToCgiPipe;
		LookupMap<const ServerBlock*> _fdToServerBlk;
};

#include "net/LookupMap.tpp"

#endif
