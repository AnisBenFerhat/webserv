# [Colors]
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
RED  		= \033[1;35m
END			= \033[0m

# Pathing variables pointing up to your root project directory
ROOT_DIR  = ..
MAIN_FILE = $(ROOT_DIR)/main.cpp
BACKUP    = $(ROOT_DIR)/main.cpp.bak
BUILT_BINARY    = $(ROOT_DIR)/webserv

.PHONY: all clean

# Master execution rule
# Usage: make TEST=poller_tester.cpp
#        make TEST=integration_test.py
all:
	@if [ -z "$(TEST)" ]; then \
		printf "${RED}Error: Please specify a tester file. Example: make run TEST=poller_tester.cpp\n${END}"; \
		exit 1; \
	fi
	@if [ ! -f "$(TEST)" ]; then \
		printf "${RED}Error: Tester file '$(TEST)' not found.\n${END}"; \
		exit 1; \
	fi
	
	@# Detect extension
	@EXT=$$(printf "$(TEST)" | grep -oE '\.[a-zA-Z0-9]+$$'); \
	if [ "$$EXT" = ".cpp" ]; then \
		printf "${YELLOW}⚡ C++ Tester Detected. Hot-swapping and building custom binary...${END}\n"; \
		cp $(MAIN_FILE) $(BACKUP) && \
		cp $(TEST) $(MAIN_FILE) && \
		($(MAKE) --no-print-directory -C $(ROOT_DIR) clean && \
			$(MAKE) --no-print-directory -C $(ROOT_DIR) || \
			(mv $(BACKUP) $(MAIN_FILE) && exit 1)) && \
		mv $(BACKUP) $(MAIN_FILE) && \
		OUT_NAME=$$(basename "$(TEST)" .cpp) && \
		mv $(BUILT_BINARY) ./$$OUT_NAME && \
		printf "${YELLOW}✅ Custom executable created: ./$$OUT_NAME${END}\n"; \
	elif [ "$$EXT" = ".py" ]; then \
		printf "${YELLOW}🐍 Python Script Detected. Pre-compiling base webserver in root...${END}\n"; \
		$(MAKE) --no-print-directory -C $(ROOT_DIR) clean && \
		$(MAKE) --no-print-directory -C $(ROOT_DIR) && \
		printf "${YELLOW}✅ Base webserver compiled. You can now execute your Python script via: python3 $(TEST)${END}\n"; \
	else \
		printf "${RED}Error: Unsupported tester file type '$$EXT'. Must be .cpp or .py${END}\n"; \
		exit 1; \
	fi

clean:
	@if [ -f "$(BACKUP)" ]; then \
		mv $(BACKUP) $(MAIN_FILE); \
	fi
	@$(MAKE) --no-print-directory -C $(ROOT_DIR) clean
	@printf "${YELLOW}Removing local generated testers...${END}\n"
	@rm -f $$(find . -maxdepth 1 -type f -not -name "Makefile" -not -name "*.cpp" -not -name "*.py")
