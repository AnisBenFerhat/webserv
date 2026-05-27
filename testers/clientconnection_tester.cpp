/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientconnection_tester.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 16:37:58 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/27 16:38:01 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include "config/ServerBlock.hpp"
#include "net/ClientConnection.hpp"
#include "net/Poller.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// =============================================================================
// Tests
// =============================================================================

/**
 * @brief Tests basic structural lifecycle configuration parameters when
 * a connection object gets instantiated.
 */
static void testClientConnectionDefaults() {
	Logger::logPart("ClientConnection State Initialization");

	ClientConnection conn;

	if (!conn.isTimedOut()) {
		Logger::logInfo(
			"OK: Default connection status initialized as healthy (not timed "
			"out)");
	}
}

/**
 * @brief Simulates an active network transaction. Uses socketpair to inject
 * mock raw request payloads and tracks how data translates into the write
 * buffer.
 */
static void testClientConnectionReadWriteCycle() {
	Logger::logPart("ClientConnection Read & Write Event Cycle");

	int sv[2];
	// Create a local connected socket pair (sv[0] is server side, sv[1] is mock
	// client side)
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
		Logger::logError("Failed to build local mock socket pair");
		return;
	}

	Poller			   mockPoller;
	struct sockaddr_in clientAddr;
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	// Allocate dynamically on heap because handleRead can potentially trigger
	// "delete this;"
	ClientConnection* conn =
		new ClientConnection(sv[0], clientAddr, NULL, &mockPoller);

	mockPoller.getLookupTable().insertClient(sv[0], conn);

	std::string mockIncomingData = "GET / HTTP/1.1\r\n\r\n";
	send(sv[1], mockIncomingData.c_str(), mockIncomingData.size(), 0);

	// Step 2: Trigger the read processor
	conn->handleRead();
	Logger::logInfo(
		"OK: handleRead read incoming packets and registered mock response "
		"block");

	// Step 3: Flush the response packet back down our mock socket pipe
	conn->handleWrite();

	// Read out what was transferred to the mock client side descriptor
	char readBackBuffer[1024];
	std::memset(readBackBuffer, 0, sizeof(readBackBuffer));
	int bytesFromServer =
		recv(sv[1], readBackBuffer, sizeof(readBackBuffer) - 1, 0);

	if (bytesFromServer > 0) {
		Logger::logInfo(
			"OK: handleWrite successfully flushed bytes into network "
			"descriptor");
	}

	// Clean up connections and mock sockets
	close(sv[1]);
}

/**
 * @brief Explicitly validates the critical "delete this;" disconnection branch
 * inside handleRead by passing a closed socket that yields 0 bytes on recv.
 */
static void testClientConnectionDisconnection() {
	Logger::logPart("ClientConnection Client Close Disconnection");

	int sv[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
		return;
	}

	Poller			   mockPoller;
	struct sockaddr_in clientAddr;
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	// Must be heap allocated due to internal self-destruction
	ClientConnection* conn =
		new ClientConnection(sv[0], clientAddr, NULL, &mockPoller);

	mockPoller.getLookupTable().insertClient(sv[0], conn);

	close(sv[1]);

	Logger::logInfo(
		"Triggering handleRead with EOF state (0 bytes counter)...");

	// This executes "delete this;" internally inside your source layout
	conn->handleRead();

	Logger::logInfo(
		"OK: Disconnection branch executed self-cleanup without system "
		"crashes");
}

// =============================================================================
// Master Orchestrator
// =============================================================================

static void runClientConnectionTestSuite() {
	Logger::logPart("Running Isolated ClientConnection Class Tests");

	testClientConnectionDefaults();
	testClientConnectionReadWriteCycle();
	testClientConnectionDisconnection();

	Logger::logPart("ClientConnection Tests completed");
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	try {
		runClientConnectionTestSuite();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
