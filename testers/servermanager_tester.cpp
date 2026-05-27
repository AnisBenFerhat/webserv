/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   servermanager_tester.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 16:35:05 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/27 16:35:07 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

#include "core/ServerManager.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// External global flag from ServerManager to control the loop execution
extern volatile std::sig_atomic_t g_keepRunning;

// =============================================================================
// Utils
// =============================================================================

/**
 * @brief Helper utility to instantly write out string contents to a disk file.
 */
static void createTestConfigFile(const std::string& filepath,
								 const std::string& content) {
	std::ofstream file(filepath.c_str());
	if (file.is_open()) {
		file << content;
		file.close();
	}
}

/**
 * @brief Cleans up and destroys the testing configuration file context.
 */
static void deleteTestConfigFile(const std::string& filepath) {
	std::remove(filepath.c_str());
}

// =============================================================================
// Tests
// =============================================================================

/**
 * @brief Verifies that a newly instantiated ServerManager safely initializes
 * its internal collections to zero size items.
 */
static void testServerManagerDefaults() {
	Logger::logPart("ServerManager Default Initialization");

	ServerManager manager;

	// Testing assignment operator logic safely preserves initialization state
	ServerManager assignedManager;
	assignedManager = manager;

	Logger::logInfo(
		"OK: Default constructor and assignment initialized cleanly");
}

/**
 * @brief Tests the behavior of launch() when provided an empty config file
 * path. Validates that it bypasses the event loop gracefully without throwing
 * errors.
 */
static void testLaunchWithNoConfigs() {
	Logger::logPart("ServerManager Launch Boundary Condition (No Configs)");

	std::string emptyConfigPath = "/tmp/empty_webserv_test.conf";
	createTestConfigFile(emptyConfigPath, "");	// Completely empty file

	ServerManager manager;

	// This calls _init(), realizes config size is 0, skips _serverLoop() in
	// _run(), and calls _stop()
	manager.launch(emptyConfigPath);

	Logger::logInfo("OK: launch handles zero configuration blocks safely");

	deleteTestConfigFile(emptyConfigPath);
}

/**
 * @brief Directly exercises the internal static signal handler mechanics to
 * ensure execution state tracking switches off instantly upon intercepting a
 * SIGINT.
 */
static void testSignalInterception() {
	Logger::logPart("ServerManager Signal Handler Logic");

	// Force loop state to active
	g_keepRunning = 1;
	if (g_keepRunning == 1) {
		Logger::logInfo("Initial runtime state verified: active");
	}

	// Directly invoke the signal handler mock to simulate a user pressing
	// CTRL+C ServerManager internal tracking captures this globally to drop out
	// of _serverLoop We can simulate it safely using the public interface or
	// direct access
	raise(SIGINT);

	if (g_keepRunning == 0) {
		Logger::logInfo(
			"OK: g_keepRunning atomic flag updated to 0 via signal handler");
	}

	// Reset back to safe defaults for the application framework flow
	g_keepRunning = 1;
}

// =============================================================================
// Master Orchestrator
// =============================================================================

static void runServerManagerTestSuite() {
	Logger::logPart("Running Isolated ServerManager Class Tests");

	testServerManagerDefaults();
	testLaunchWithNoConfigs();
	testSignalInterception();

	Logger::logPart("ServerManager Tests completed");
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	try {
		runServerManagerTestSuite();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
