/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcplistener_tester.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 16:37:39 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/27 16:37:41 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>

#include "net/TcpListener.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// =============================================================================
// Tests
// =============================================================================

/**
 * @brief Verifies that passing an already active file descriptor directly
 * populates the class state immediately without triggering real syscall chains.
 */
static void testTcpListenerSocketAdoption() {
	Logger::logPart("TcpListener Active Socket Adoption");

	int mockFd = 42;

	// Instantiate using the streamlined direct descriptor constructor
	TcpListener listener(mockFd);

	if (listener.getSocket() == 42) {
		Logger::logInfo(
			"OK: Constructor successfully assumes direct control of raw FD 42");
	}

	const sockaddr_in& addr = listener.getAddress();
	// Validate the struct was zeroed out inside the constructor
	if (addr.sin_port == 0 && addr.sin_family == 0) {
		Logger::logInfo(
			"OK: Accompanying address block cleanly zero-initialized");
	}
}

/**
 * @brief Validates manual setter/getter updates on both the address struct
 * and the active tracking descriptor index.
 */
static void testTcpListenerGettersAndSetters() {
	Logger::logPart("TcpListener Data Layer Injections");

	// Instantiates silently now (no implicit background bind/socket syscalls)
	TcpListener listener;

	// Explicit override inputs
	listener.setSocket(99);

	struct sockaddr_in customAddr;
	customAddr.sin_family	   = AF_INET;
	customAddr.sin_port		   = htons(1234);
	customAddr.sin_addr.s_addr = INADDR_ANY;
	listener.setAddress(customAddr);

	// Output integrity assertions
	if (listener.getSocket() == 99) {
		Logger::logInfo(
			"OK: setSocket mutated target file descriptor layer successfully");
	}
	if (listener.getAddress().sin_port == htons(1234)) {
		Logger::logInfo(
			"OK: setAddress updated target network ports structure mapped "
			"layout");
	}

	// Explicit close teardown tracking check
	listener.closeTcp();
	if (listener.getSocket() == -1) {
		Logger::logInfo("OK: closeTcp safely resets tracked descriptor to -1");
	}
}

/**
 * @brief Assures data alignments transfer cleanly through copying context
 * definitions.
 */
static void testTcpListenerCopySemantics() {
	Logger::logPart("TcpListener Structural Memory Copies");

	struct sockaddr_in mockAddr;
	mockAddr.sin_family = AF_INET;
	mockAddr.sin_port	= htons(5678);

	TcpListener original(88, mockAddr);

	TcpListener copyConstructed(original);

	// Reset original descriptor parameters to mock an assignment source
	original.setSocket(88);
	TcpListener assignmentInjected;
	assignmentInjected = original;

	if (copyConstructed.getSocket() == 88 &&
		copyConstructed.getAddress().sin_port == htons(5678)) {
		Logger::logInfo(
			"OK: Copy constructor duplicates descriptor and address details");
	}
	if (assignmentInjected.getSocket() == 88 &&
		assignmentInjected.getAddress().sin_port == htons(5678)) {
		Logger::logInfo(
			"OK: Copy assignment operator syncs properties flawlessly");
	}
}

/**
 * @brief Validates our professional RAII resource ownership handoff pattern.
 * Ensures the source object's descriptor drops to -1 when copied so that its
 * destructor doesn't accidentally kill the socket needed by the clone
 * destination.
 */
static void testTcpListenerResourceStealing() {
	Logger::logPart("TcpListener RAII Resource Ownership Handoff");

	TcpListener sourceBlock(
		55);  // Simulates an active socket block descriptor directly

	// Copying triggers our custom resource ownership transfer mechanism
	TcpListener destinationBlock(sourceBlock);

	if (destinationBlock.getSocket() == 55 && sourceBlock.getSocket() == -1) {
		Logger::logInfo(
			"OK: Resource stealing successful. Destination took ownership of "
			"FD 55, Source reset safely to -1");
	}
}

// =============================================================================
// Master Orchestrator
// =============================================================================

static void runTcpListenerTestSuite() {
	Logger::logPart("Running Isolated TcpListener Class Tests");

	testTcpListenerSocketAdoption();
	testTcpListenerGettersAndSetters();
	testTcpListenerCopySemantics();
	testTcpListenerResourceStealing();

	Logger::logPart("TcpListener Tests completed");
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	try {
		runTcpListenerTestSuite();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
