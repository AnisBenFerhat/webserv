/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_tester.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flebrun <flebrun@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 16:34:08 by flebrun           #+#    #+#             */
/*   Updated: 2026/05/27 16:34:19 by flebrun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

#include "config/Config.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// =============================================================================
// Utils
// =============================================================================

/**
 * @brief Helper utility to instantly write out string contents to a disk file.
 * This simulates our configuration file on disk.
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
 * @brief Verifies default constructor parameters initialized on fresh
 * instantiations. Pure Input/Output validation.
 */
static void testConfigDefaults() {
	Logger::logPart("Config Default State Initialization");

	Config config;

	if (config.getPort() == 8080)
		Logger::logInfo("OK: default port initialized to 8080");
	if (config.getClientMaxBodySize() == DEFAULT_MAX_BODY_SIZE)
		Logger::logInfo(
			"OK: max body size initialized to DEFAULT_MAX_BODY_SIZE");
	if (config.getHost().empty())
		Logger::logInfo("OK: default host string is completely empty");
}

/**
 * @brief Tests structural data tracking inside getters, setters, and search
 * logic.
 */
static void testConfigSettersAndGetters() {
	Logger::logPart("Config Setters / Getters & Search Integrity");

	Config config;

	// Injecting known inputs directly
	config.setPort(4242);
	config.setHost("127.0.0.1");
	config.setClientMaxBodySize(2048);
	config.addServerBlockName("localhost");
	config.addServerBlockName("example.com");
	config.addErrorPage(404, "/errors/404.html");

	// Verifying known outputs
	if (config.getPort() == 4242)
		Logger::logInfo("OK: setPort successfully updated value to 4242");
	if (config.getHost() == "127.0.0.1")
		Logger::logInfo("OK: setHost successfully updated value to 127.0.0.1");
	if (config.getClientMaxBodySize() == 2048)
		Logger::logInfo(
			"OK: setClientMaxBodySize successfully updated value to 2048");

	// Testing the boolean lookup array logic
	if (config.hasServerBlockName("localhost"))
		Logger::logInfo("OK: hasServerBlockName locates matching elements");
	if (!config.hasServerBlockName("missing.org"))
		Logger::logInfo(
			"OK: hasServerBlockName safely reports missing domains");

	if (config.getErrorPages().at(404) == "/errors/404.html")
		Logger::logInfo("OK: custom error pages added and mapped correctly");
}

/**
 * @brief Simulates file execution patterns when attempting to access a
 * nonexistent layout path.
 */
static void testMissingFileHandling() {
	Logger::logPart("Parse Attempt with Missing Configuration File");

	// Input: A bad file path
	std::vector<Config> configs =
		Config::parseConfig("/tmp/ghost_file_does_not_exist.conf");

	// Output: Should be an empty vector safely, without throwing unhandled
	// exceptions
	if (configs.empty())
		Logger::logInfo(
			"OK: missing file handled gracefully, returned empty configuration "
			"vector");
}

/**
 * @brief Validates processing performance on structured Nginx server blocks.
 */
static void testParseValidServerBlock() {
	Logger::logPart("Valid Nginx Server Block Discovery");

	std::string testPath = "/tmp/webserv_isolated_test.conf";

	// Simulated mock configuration string input
	std::string rawConfig =
		"server {\n"
		"}\n"
		"server {\n"
		"}\n";

	createTestConfigFile(testPath, rawConfig);

	// Run the isolated parser loop
	std::vector<Config> configs = Config::parseConfig(testPath);

	Logger::logInfo("Detected valid block entities: " +
					Convertor::uIntToStr(configs.size()));

	// Output validation: Based on your Config.cpp logic, it finds 2 server
	// contexts
	if (configs.size() == 2)
		Logger::logInfo(
			"OK: multiple 'server {' blocks discovered and parsed "
			"successfully");

	deleteTestConfigFile(testPath);
}

// =============================================================================
// Master Orchestrator
// =============================================================================

static void runConfigTestSuite() {
	Logger::logPart("Running Isolated Config Class Tests");

	testConfigDefaults();
	testConfigSettersAndGetters();
	testMissingFileHandling();
	testParseValidServerBlock();

	Logger::logPart("Config Tests completed");
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	try {
		runConfigTestSuite();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
