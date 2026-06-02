# QA-Testing Branch Workflow Guide

This guide explains how developers should use the permanent `QA-testing` branch to share code with testers without messy copy-pasting, while keeping the main `dev` and personal feature branches 100% clean and pristine.


## Core Concept: The One-Way Street
The `QA-testing` branch is an isolated aggregator. 
* Code flows INTO `QA-testing` from your features.
* Code NEVER flows from `QA-testing` back into `dev` or your features. 

## 1. Sharing Local Code Safely with Testers
If you have written a new feature on a branch (e.g., `login/feature`) and want to test it before it gets merged into the main project:

### Step A: Commit your work safely on your feature branch
---
Make sure your work is saved locally on your own branch if you're unsure about it being modified. 
```bash
git checkout [login/feature]
git add .
git commit -m "[feat]: implement ..."
```
(Your feature branch is now locked, completely safe, and hasn't been pushed to the remote server yet, you can still `git add` more afterwards).


### Step B: Merge locally into `QA-testing` and Push
---
Switch over to the testing branch, pull down any updates your teammates might have pushed, and pull your feature in.
```bash
# 1. Switch to QA-testing
git checkout QA-testing

# 2. Get the latest changes from other devs/testers
git pull origin QA-testing

# 3. Merge your feature locally (Optionally use --squash to keep it to 1 clean commit)
git merge [login/feature]

# 4. Push to the server so testers can see it
git push origin QA-testing --force-with-lease
```

### Step C: Return to your clean feature branch
```bash
git checkout [login/feature]
```
Result: Your testers have your code on the server via `QA-testing`. Your feature-login branch is still completely clean, untouched, and ready for your development team.

## 2. Updating QA-testing when `origin/dev` is Ahead

If other developers have pushed new features to the main `origin/dev branch`, your `QA-testing` branch will fall behind. Here is how to catch it up safely without losing your ongoing tests.
```bash
# 1. Switch to the testing branch
git checkout QA-testing

# 2. Fetch the latest updates from the server
git fetch origin

# 3. Replay your QA-testing commits on top of the latest development code
git rebase origin/dev
```
- Note: If Git says "Current branch QA-testing is up to date", it means you already have all the latest code from `dev`.
- If you rebase and it says "Your branch is ahead of 'origin/QA-testing' by X commits", simply push it to update the testers:
```bash
git push origin QA-testing
```

## 3. Creating Tests
To implement new tests, instead of storing multiples `main.cpp`, the goal will be to create `[class]_tester.cpp` files into the the `testers` directory to use them just like as a `main.cpp` but is more detailed name.

### Step A: Write down the tests
---
To make the task less time consuming, here is a template of a unit test that you can use to feed an AI with it to create new units tests depending on your `.hpp` and `.cpp` files included. The best is just to create a new conversation and copy paste it to have all the context needed, nothing more to avoid hallucinations.


#### AI Prompt Instructions (Copy everything below this line)
```text
You are an expert C++98 QA Automation Engineer. I am going to provide you with a Reference Template that demonstrates our exact project style, logging wrappers, file mocking utilities, and structural patterns for writing unit tests. 

### Rules for Generating New Tests:
1. Do not use external testing frameworks (like Google Test or Catch2). Use vanilla C++98 assertions and state validation matching the template below.
2. Every test phase must begin with `Logger::logPart("Context Name");` for better logs readibility.
3. Positive state validations must be evaluated via explicit `if` conditions and log success using `Logger::logInfo("OK: ...");`.
4. If a test simulates file reading, use the `createTestConfigFile` and `deleteTestConfigFile` paradigms to write mock data to disk safely.
5. Wrap the test suite executor in a master function and catch standard exceptions via `Logger::logError()`.

Here is the code template reflecting our framework style:
```
```cpp
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

    // Output validation: Based on your Config.cpp logic, it finds 2 server contexts
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
```

### Step B: How to use your testers

Inside the `testers` directory, you will find a Makefile that has the goal to create the desired `webserv` program but using your provided `class_tester.cpp` instead of the original `main.cpp`. It can even work with your `script_tester.py` if you want to run it using the original `main.cpp`. To do so, here are the differents uses:
```bash
# 1. Creating the program using your tester.cpp as a main.cpp
make TEST=class_tester.cpp

# 2. Running the program like normally
./class_tester

# 3. Creating the base program using the regular main.cpp
make TEST=script_tester.py

# 4. Running the python script on the base program
python3 script_tester.py

# 5 Clean every executables
make clean
```

### Step B: How to ship the test cases

```bash
# Commit this file directly to the QA branch using:
git add [your-test.cpp]
git commit -m "Added test for `Class` class"
git push 

# Switch back to your branch 
git switch [login/your-branch]
```
