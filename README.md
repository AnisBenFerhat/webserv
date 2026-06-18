*This project has been created as part of the 42 curriculum by aben-fer, elkanega, flebrun.*

## Description

Webserv is an asynchronous HTTP/1.1 web server built from scratch in C++98. Our project features single-threaded I/O multiplexing (`poll()`) to coordinate concurrent network socket connections, file streams, and dynamic CGI subprocesses without stalling the primary execution loop. It was modeled after the event-driven architecture of Nginx. It also includes an interactive administrative dashboard and a file-backed persistent session routing architecture. Our project complies with HTTP/1.1 (RFC 2616) and CGI/1.1 (RFC 3875) specifications. It handles non-blocking network communication to serve static web spaces, executes Common Gateway Interface (CGI) scripts, and manages client file life cycles without relying on modern standard components or external web frameworks.

### Objectives

- Handle concurrent client connections inside a single-threaded loop using kernel-level monitoring (`poll()`).

- Parse, validate, and respond to HTTP structural methods (`GET`, `POST`, `DELETE`).

- Support Virtual Hosting matrices, route-specific context parameters, and dynamic cross-language script evaluation via CGI pipes.

- Protect host environments by identifying and dropping malformed protocol states, integer overflow attacks, or over sized payload bodies before they trigger performance regressions.

### Inbound Traffic Request Pipeline

```
 \[ Client Request \]
		 │
		 ▼
   TcpListener ──► Opens sockets, binds to host ports, and executes accept()
		 │
		 ▼
	 Poller ────► Monitors active Fds via poll() without blocking
		 │
		 ▼
   Connection ──► Buffers raw bytes sequentially from client sockets
		 │
		 ▼
   HttpParser ──► Tokenizes byte streams into structured request states
		 │
		 ▼
	 Router ────► Evaluates location blocks using Longest Prefix Matching
		 │
		 ▼
   Dispatcher ──► Diverts flow to Static Asset Files or CGI Processes
		 │
		 ▼
  HttpResponse ─► Serializes data headers & payloads to stream over wire
```

## Instructions

### Compilation

The compilation framework is governed by a standard-compliant `Makefile`. Execute the following controls inside your terminal:

```
make          \# Compiles the optimized webserver binary executable (webserv)
make clean    \# Strips away internal object files (.o)
make fclean   \# Performs full deletion of object files and target executable
make re       \# Forces a clean recompilation of the architecture from scratch
```

### Execution

Initialize the network environment by supplying a valid path configuration map file. If an implicit pathway mapping parameter is omitted, the engine falls back onto the repository default profile:

```
./webserv \[path\_to\_config.conf\]

\# Example (Default Execution Profile)
./webserv conf/default.conf
```

- Raw connection updates, active traffic streams, routing updates, and diagnostic state flags are piped to stdout/stderr.

- Pass an interrupt signal (`Ctrl + C`) to terminate the primary loop execution. The server catches the exception, flushes active socket buffers, and releases bound system ports.

### Testing & Navigation

Launch an internet browser and target the local network address workspace loopback coordinates:  `http://127.0.0.1:8080` or `http://localhost:8080`

The standard front-end application allows developers to exercise the three core HTTP verbs over the network:

- **1. Fetch a Resource (GET):** Request, index, and load static assets from local document roots.

- **2. Upload a Resource (POST):** Stream data files via interactive boundary forms. Payload bodies are captured and committed inside `www/uploads/`.

- **3. Delete a Resource (DELETE):** Clear uploaded files from the server via the dashboard UI.

## Key Features Demo

### Cookie & Session Management

The core engine supports persistent session tracking through server-side JSON cross-checking and HTTP header updates:

- Point your browser to the **Admin Portal** (`http://localhost:8080/admin/index.html`) by using the login button on the top right corner of the home page. The access credentials are pre-filled.

- When authorized, a unique `session\_id` cookie token is injected into the response headers (`Set-Cookie`) and cached by your browser with security parameters (`HttpOnly; Max-Age=3600`).

- Clicking **Logout** strips the token from the backend `sessions.json` table and feeds a clearing token (`Max-Age=0`) over the wire. Any raw script attempts to bypass the portal and load the administration area will be dropped and intercepted, forcing an automatic 302 redirection back to the index home page.

### Error Redirection Mapping

When a client requests a corrupted path, broken route, or non-existent asset workspace (e.g., `/this-page-does-not-exist`), the routing matrix intercepts the execution error and returns an HTTP Error Page.

### Virtual Hosting

Webserv evaluates multiple distinct virtual host profiles running simultaneously across shared interface ports by examining the incoming `Host` request header parameter token.

To evaluate this functionality, register domain strings inside the machine's host mapping directory (`/etc/hosts`):

```
127.0.0.1		site1.com
127.0.0.1		site2.com
```

You can then declare overlapping virtual blocks in the server file like this:

```
http \{
	server \{
		listen 8080;
		server\_name site1.com;
		root www/site1;
		index index.html;
	\}
	server \{
		listen 8080;
		server\_name site2.com;
		root www/site2;
		index index.html;
	\}
\}
```

## Edge Case Testing (Parser Debug Mode)

To validate structural parsing logic, the architecture provides a comprehensive adversarial test block configuration matrix located at `conf/errors\_checker.conf`. This block targets syntax errors such as missing semicolons, out-of-bounds ports, negative client bodies, structural duplicates, and unclosed scope frames.

By default, syntax violations throw a `std::runtime\_error` and halt initialization. To shift the server into an advanced **Visual Tree Debugger Mode**, execute a global find-and-replace command within the parser file layer using VSCode replace tool:

1. **Find:** `throw std::runtime\_error`

2. **Replace with:** `\_debug`

3. **Inject** the tracking routine below into your engine code:

```
// Add to src/config/ConfigParser.cpp
void ConfigParser::\_debug(const std::string& msg) \{
	static const std::string TREE\_LOOKUP\[3\]\[3\] = \{
		\{". "\}, \{"|  |--- "\}, \{"|--- "\}
	\};

	int row = 0;
	if (\_parsingState == SERVER\_CONTEXT) \{
		row = 1;
	\}
	if (\_parsingState == LOCATION\_CONTEXT) \{
		row = 2;
	\}

	std::string asciiTree = TREE\_LOOKUP\[row\]\[0\];
	std::string line = Convertor::uIntToStr(\_lineCounter + \_parsingState - 2);
	std::string fullMsg = asciiTree + msg + " at line \[" + line + "\]";

	\_log("|");
	Logger::logWarning("\`--- " + fullMsg);
	\_log("");
\}

// Add to includes/config/ConfigParser.hpp
void \_debug(const std::string& msg);
```

### Visual System Architecture

```
TcpListener (socket(), bind(), listen()) → Poller (accept(), poll()) → ClientConnection (recv() buffer) → HttpRequestParser (HttpRequest) → RequestRouter (LocationBlock via Longest Prefix Matching) → Dispatcher (Static vs CGI evaluation) → HttpResponse (serialize()).
```

## Static Error Template Catalog

| Status Code | Structural Description | Architectural Trigger Condition |
| - | - | - |
| **400 Bad Request** | Syntax error parsing inbound data streams. | `HttpRequestParser` hits unresolvable tokenization boundaries or malformed syntax headers. |
| **403 Forbidden** | Missing resource execution permissions. | Directory listings are deactivated or access configurations deny the request. |
| **404 Not Found** | Target resource location mapping failed. | `RequestRouter` fails to match an existing file or explicit virtual host path. |
| **405 Method Not Allowed** | Request method rejected on specified route. | (`GET`/`POST`/`DELETE`) violates the `LocationBlock` limits. |
| **413 Payload Too Large** | Entity body bounds exceeded. | Incoming payload metrics exceed `client\_max\_body\_size`. |
| **500 Internal Error** | Server-side execution exception. | A core processing failure or unmanaged resource breakdown. |
| **502 Bad Gateway** | Unreadable or malformed CGI execution output. | C++ pipe reader intercepts invalid header blocks from an executed script. |
| **504 Gateway Timeout** | Script runtime execution window expired. | An active CGI child process fails to output data before poller's timeout. |


## Resources

- **CGI Specifications:** [RFC 3875 - Common Gateway Interface v1.1](https://datatracker.ietf.org/doc/html/rfc3875#section-3.2)

- **I/O Multiplexing:** [Linux man-pages: poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html)

- **Nginx Architecture Reference:** [Nginx Official Configuration Documentation](https://nginx.org/en/docs/beginners_guide.html#conf_structure)

- **HTTP Protocol:** [MDN Mozilla](https://developer.mozilla.org/en-US/docs/Web/HTTP)

- **HTTP/1.1 RFC2616:** [IETF Datatracker](https://datatracker.ietf.org/doc/rfc2616/)

- **HTTP/1.1 Semantics:** [RFC 7230 - Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230#section-2.7)

- **Socket Programming:** [GeeksForGeeks](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)

- **URL Encoding Mechanisms:** [RFC 3986 - Uniform Resource Identifier (URI) Generic Syntax](https://datatracker.ietf.org/doc/html/rfc3986#section-2.1)

AI tools, such as Gemini and Claude, were used to optimize code quality, architecture, ensure C++ Standard 98 compliance, and to help with writing testing scripts.
