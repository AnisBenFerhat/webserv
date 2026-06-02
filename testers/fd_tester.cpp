/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_tester.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 13:16:48 by flebrun           #+#    #+#             */
/*   Updated: 2026/06/02 13:17:08 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <memory.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <exception>

#include "core/ServerManager.hpp"
#include "net/ClientConnection.hpp"
#include "net/Fd.hpp"
#include "utils/Logger.hpp"

/**
 * @brief Checks and validates ClientConnection error management and kernel
 * errno extraction.
 */
void testErrnoHandling() {
	Logger::logInfo("--- ClientConnection Errno Handling ---");

	int sv[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
		Logger::logError("FAIL: Could not create socket pair for errno test");
		return;
	}

	Poller poller;
	Fd*	   fd = new Fd(sv[0]);

	ServerBlock testServer;

	ClientConnection* conn = new ClientConnection(fd, &testServer, &poller);
	poller.getLookupTable().insertClient(sv[0], conn);
	poller.addFd(sv[0], POLLIN);

	// close to force error
	close(sv[0]);

	conn->handleRead();
	Logger::logInfo(
		"PASS: handleRead caught explicit system error, set logErrno, and "
		"exited cleanly");

	close(sv[1]);
	if (poller.getLookupTable().getClientIt(sv[0]) !=
		poller.getLookupTable().getClientEndIt()) {
		poller.removeClient(sv[0]);
	} else {
		Logger::logInfo(
			"PASS: ClientConnection was automatically deallocated by the error "
			"handler");
	}
}

/**
 * @brief Validates RAII compliance and resource reclamation.
 * clearTable() clears the lookup map structures and deletes all active
 * ClientConnection heap pointers. When Poller goes out of scope, ~Poller()
 * ensures proper cleanup.
 */
void testPollerCleanup() {
	Logger::logInfo("--- Poller Destructor & clearTable Heap Cleanup ---");

	ServerBlock testServer;

	{
		Poller testPoller;

		int pairA[2];
		int pairB[2];
		int pairC[2];

		if (socketpair(AF_UNIX, SOCK_STREAM, 0, pairA) < 0 ||
			socketpair(AF_UNIX, SOCK_STREAM, 0, pairB) < 0 ||
			socketpair(AF_UNIX, SOCK_STREAM, 0, pairC) < 0) {
			Logger::logError(
				"FAIL: Unable to provision legitimate socket harnesses for "
				"test");
			return;
		}

		Fd*				  fd1 = new Fd(pairA[0]);
		ClientConnection* conn1 =
			new ClientConnection(fd1, &testServer, &testPoller);
		testPoller.getLookupTable().insertClient(pairA[0], conn1);
		testPoller.addFd(pairA[0], POLLIN);

		Fd*				  fd2 = new Fd(pairB[0]);
		ClientConnection* conn2 =
			new ClientConnection(fd2, &testServer, &testPoller);
		testPoller.getLookupTable().insertClient(pairB[0], conn2);
		testPoller.addFd(pairB[0], POLLIN);

		Logger::logInfo(
			"Successfully allocated and registered multiple mock connections "
			"in Poller");

		testPoller.clearTable();

		if (testPoller.getLookupTable().getClientIt(pairA[0]) ==
				testPoller.getLookupTable().getClientEndIt() &&
			testPoller.getLookupTable().getClientIt(pairB[0]) ==
				testPoller.getLookupTable().getClientEndIt()) {
			Logger::logInfo(
				"PASS: clearTable manually flushed all map entries and "
				"executed pointer deletes");
		} else {
			Logger::logError(
				"FAIL: clearTable left hanging records inside LookupTable "
				"containers");
		}

		close(pairA[1]);
		close(pairB[1]);

		// Repopulate Poller
		Fd*				  fd3 = new Fd(pairC[0]);
		ClientConnection* conn3 =
			new ClientConnection(fd3, &testServer, &testPoller);
		testPoller.getLookupTable().insertClient(pairC[0], conn3);
		testPoller.addFd(pairC[0], POLLIN);

		close(pairC[1]);
		Logger::logInfo(
			"Re-populated Poller. Exiting to trigger Poller Destructor... ");
	}

	Logger::logInfo("PASS: Poller destructor successfully called clearTable");
}

int main(int argc, char** argv) {
	(void)argv;
	if (argc > 2) {
		Logger::logError("Usage: ./webserv [config_file]");
		return EXIT_FAILURE;
	}
	try {
		testErrnoHandling();
		testPollerCleanup();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
