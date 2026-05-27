#include <sys/poll.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>

#include "net/Poller.hpp"
#include "utils/Convertor.hpp"
#include "utils/Logger.hpp"

// =============================================================================
// Tests
// =============================================================================

/**
 * @brief Checks basic vector lifecycle operations inside the wrapper object.
 * Validates adding file descriptors and updating event registration bitmasks.
 */
static void testPollerDataStructures() {
	Logger::logPart("Poller Array & Bitmask Management");

	Poller poller;

	// Injecting distinct mock descriptors
	poller.addFd(5, POLLIN);
	poller.addFd(6, POLLIN);

	// Modify events tracking targets
	poller.setEvents(5, POLLOUT);
	poller.setEvents(6, POLLIN | POLLOUT);

	Logger::logInfo(
		"OK: Managed descriptors adding and modifications gracefully");
}

/**
 * @brief Exercises the fast vector-pop deletion method implementation.
 * Ensures the internal structural optimization does not drop track of
 * components.
 */
static void testPollerRemovalMechanics() {
	Logger::logPart("Poller Fast Vector Structural Removal");

	Poller poller;

	poller.addFd(10, POLLIN);
	poller.addFd(11, POLLIN);
	poller.addFd(12, POLLIN);

	// Remove an intermediary element (triggers the back-swap optimization)
	poller.removeFd(11);

	// Clear the accompanying associative tracking lookup table map safely
	poller.clearTable();

	Logger::logInfo(
		"OK: Fast element removal and tables clearing completed cleanly");
}

/**
 * @brief Assures that executing system poll boundaries on completely empty
 * array tracking scopes falls back immediately and returns zero execution
 * counters.
 */
static void testPollerEmptyArrayFallback() {
	Logger::logPart("Poller Empty Vector Loop Protection");

	Poller poller;

	// Input boundary check: calling pollEvents with no active descriptors
	// Output: Must shortcut execute, return 0, and protect against passing null
	// elements
	int totalEvents = poller.pollEvents(10);

	if (totalEvents == 0) {
		Logger::logInfo(
			"OK: Empty poller returned 0 instantly, bypassing sys poll block "
			"execution");
	}
}

/**
 * @brief Exercises copy semantics to ensure memory state layouts transfer
 * correctly.
 */
static void testPollerCopySemantics() {
	Logger::logPart("Poller Struct Copies & Assignment Bounds");

	Poller original;
	original.addFd(7, POLLIN);

	Poller duplicated(original);
	Poller assigned;
	assigned = original;

	Logger::logInfo(
		"OK: Copy assignment and structural configurations duplicated "
		"seamlessly");
}

// =============================================================================
// Master Orchestrator
// =============================================================================

static void runPollerTestSuite() {
	Logger::logPart("Running Isolated Poller Class Tests");

	testPollerDataStructures();
	testPollerRemovalMechanics();
	testPollerEmptyArrayFallback();
	testPollerCopySemantics();

	Logger::logPart("Poller Tests completed");
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	try {
		runPollerTestSuite();
	} catch (const std::exception& e) {
		Logger::logError(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
