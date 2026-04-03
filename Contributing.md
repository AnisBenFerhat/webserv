# Webserv Project Standards

This document defines the professional standards and workflow for the **webserv** project.
Adhering to these rules ensures a maintainable, professional, and stable codebase based on Clean Code principles.

---

# I. Project Structure

The project is organized into independent modules.
Each module has a dedicated responsibility and must remain isolated to preserve clear separation of concerns.

| Directory | Description |
|-----------|-------------|
| `core/` | Global orchestration logic and `ServerManager`, which drives the server lifecycle. |
| `net/` | Low-level network primitives such as listeners, event loop management, and active client connections. |
| `config/` | Configuration parsing, directive validation, and in-memory configuration models. |
| `http/` | HTTP protocol logic: request parsing, routing, method handling, and response generation. |
| `cgi/` | Child process execution, CGI environment construction, and pipe communication. |
| `errors/` | Custom exceptions and centralized internal error definitions. |
| `utils/` | Shared utilities such as logging, string helpers, and time-related tools. |
| `include/` | Mirror of the module structure above for all header files (`.hpp`). |
| `src/` | Mirror of the module structure above for all implementation files (`.cpp`). |
| `conf/` | Server configuration files (`.conf`) and dedicated test scenarios. |
| `www/` | Document root for static content and virtual host testing. |
| `cgi-bin/` | Isolated directory for executable CGI scripts. |

---

# II. Base Project Preparation

Before parallel development begins, the project must establish a shared technical foundation.

This preparatory phase defines the common objects that all modules will rely on.

---

## 2.1 Data Models

These classes act as passive containers used to circulate structured information between modules.

- **Config**
  Stores the full parsed server configuration and provides the global configuration entry point in memory.

- **ServerBlock**
  Represents one virtual server and stores directives such as `listen`, `server_name`, `host`, and `error_page`.

- **LocationBlock**
  Defines routing rules for a specific URI prefix, including `root`, `index`, allowed methods, CGI extensions, and body limits.

- **HttpRequest**
  Represents a fully parsed HTTP request containing method, URI, version, headers, and body.

- **HttpResponse**
  Represents the HTTP response being built before transmission.

- **HttpStatus**
  Centralized enumeration of HTTP status codes and associated standard messages.

---

## 2.2 Runtime Core Objects

These objects structure the active runtime of the server.

- **ServerManager**
  Main orchestrator of the project. Handles initialization, runtime execution, and shutdown.

- **Connection**
  Represents one active client session and encapsulates socket state, buffers, request, response, and connection lifecycle.

- **Poller**
  Central abstraction for event monitoring and file descriptor management.

- **TcpListener**
  Represents one listening socket responsible for accepting new connections.

- **Router**
  Matches requests against configuration and determines which processing path must be applied.

- **CgiProcess**
  Encapsulates CGI child process execution, pipes, and runtime state.

---

## 2.3 Cross-module Tools

These tools are shared by the whole project.

- **Logger**
  Centralized logging interface for runtime visibility.

- **Exceptions**
  Shared exception hierarchy used for critical failures.

- **Makefile**
  Common build system shared by all contributors.

---

# III. Makefile Standards

The Makefile must remain scalable, readable, and easy to maintain.

---

## 3.1 Directory Variables

Each module path must be declared through dedicated variables.

This prevents hardcoded paths inside `SRCS` and simplifies maintenance.

### Example

```makefile
CORE_DIR   = src/core
NET_DIR    = src/net
CONFIG_DIR = src/config
HTTP_DIR   = src/http
CGI_DIR    = src/cgi
ERRORS_DIR = src/errors
UTILS_DIR  = src/utils

INCLUDE_DIR = include
OBJ_DIR     = obj
SRC_DIR     = src
```

---

## 3.2 Source Listing (`SRCS`)

The `SRCS` variable must follow two rules:

- alphabetical order by module directory
- `main.cpp` must always appear last

Each source file must use the directory variables.

### Example

```makefile
SRCS = $(CGI_DIR)/CgiProcess.cpp \
       $(CONFIG_DIR)/ConfigParser.cpp \
       $(CORE_DIR)/ServerManager.cpp \
       $(ERRORS_DIR)/Exceptions.cpp \
       $(HTTP_DIR)/HttpParser.cpp \
       $(NET_DIR)/Poller.cpp \
       $(UTILS_DIR)/Logger.cpp \
       main.cpp
```

---

## 3.3 Benefits

This approach guarantees:

- easier refactoring
- cleaner Makefile growth
- immediate readability
- safer module expansion

---

# IV. Work Distribution (3 Contributors)

Once the shared base is validated, development is split into three technical areas.

---

## Contributor 1 — Network and Event Loop

Responsible for the runtime engine of the server.

### Scope

- listening sockets
- multi-port accept
- event loop
- descriptor registration/removal
- connection states
- non-blocking buffers
- timeouts

### Goal

Provide a stable non-blocking runtime able to manage multiple active clients simultaneously.

### Includes

CGI pipe integration into the event loop, since pipes are also monitored descriptors.

### Tests

- connection stress
- multiple simultaneous clients
- disconnect handling
- timeout scenarios

---

## Contributor 2 — Configuration and HTTP Parsing

Responsible for transforming raw text into validated structures.

### Scope

- configuration parsing
- server/location directive validation
- longest prefix resolution
- HTTP request parsing
- headers
- body
- chunked transfer decoding

### Goal

Provide stable parsed objects:

- validated configuration
- fully parsed `HttpRequest`

### Tests

- valid and invalid configurations
- malformed requests
- chunked requests
- boundary parsing cases

---

## Contributor 3 — HTTP Handling and CGI

Responsible for producing the final server behavior.

### Scope

- GET
- POST
- DELETE
- response generation
- custom error pages
- upload handling
- file deletion
- CGI execution

### Goal

Determine what the server returns for each request.

### Includes

- CGI process execution
- environment setup
- CGI output parsing

### Tests

- functional HTTP tests
- CGI tests
- NGINX behavior comparison

---

# V. Git Workflow

To maintain a clean and linear history, use **rebase-based integration**.

One task = one branch.

---

## 5.1 Task Lifecycle

### Sync

```bash
# Return to the integration branch
git checkout dev

# Fetch all remote changes and update dev
git fetch origin
git pull --ff-only origin dev
```

### Branch

```bash
git checkout -b login/scope-short-description
```

### Work

Perform small, logical commits. Ensure your code compiles before committing.

```bash
git add <files>
git commit -m "[Type/Scope] - Clear message"
```


### Rebase

```bash
# Fetch the latest changes from origin
git fetch origin

# Replay your commits on top of the new dev
git rebase origin/dev
```

Note: If conflicts occur, resolve them, use git add <resolved-files>, and then git rebase --continue.

### Push and Pull Request

Once your history is linear and up-to-date, share your work.

```bash
git push -u origin login/scope-short-description
```

Then, open a Pull Request (PR) on GitHub from your branch to dev.

---

## 5.2 Post-Merge Cleanup

```bash
git branch -d login/scope-short-description
git push origin --delete login/scope-short-description
```

---

# VI. Clean Code and Naming Conventions

---

## 6.1 Explicit Naming

Avoid unclear abbreviations.

### Forbidden

- `req`
- `res`
- `ptr`
- `srv`
- `buf`

### Allowed

- `request`
- `response`
- `pointer`
- `server`
- `buffer`

### Exception

Short loop variables remain acceptable:

- `i`
- `j`
- `pos`
- `len`

---

## 6.2 Naming Rules

- **Classes:** PascalCase
  Example: `HttpRequest`

- **Methods / Variables:** camelCase
  Example: `parseBuffer()`

- **Private attributes:** `_camelCase`
  Example: `_clientSocket`

---

## 6.3 Core Principles

- Single Responsibility Principle
- RAII
- Orthodox Canonical Form when ownership or copy semantics matter

---

## 6.4 Documentation Briefs

Public methods in headers must include a brief description using Doxygen syntax. This ensures IDE tooltip support and allows for automatic documentation generation.

Example:

```cpp
/**
 * @brief Matches the request URI to the best available location block.
 * @param request Parsed request object.
 * @return Matching location block.
 */
```

---

# VII. Git Reference Table

| Command | Usage |
|---------|-------|
| `git reset --hard HEAD~1` | Rollback the last commit and discard changes |
| `git checkout -- <file>` | Restore one file to last committed state |
| `git log --oneline --graph` | Visualize branch history |

---
