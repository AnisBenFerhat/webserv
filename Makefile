# [Colors]
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
BLUE		= \033[1;34m
END			= \033[0m

# Executable
NAME = webserv

# Compiler
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -MMD -Iincludes

# Directories
OBJ_DIR = obj
SRC_DIR = src
CORE_DIR = src/core
ERRORS_DIR = src/errors
HTTP_DIR = src/http
UTILS_DIR = src/utils

# Files
SRCS = 	\
		$(CORE_DIR)/ServerManager.cpp \
		$(ERRORS_DIR)/ErrorCode.cpp \
		$(ERRORS_DIR)/Exceptions.cpp \
		$(HTTP_DIR)/HttpRequest.cpp \
		$(HTTP_DIR)/HttpStatus.cpp \
		$(UTILS_DIR)/Logger.cpp \
		main.cpp \

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(END)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Webserv starter is ready!$(END)"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)Compiling $<...$(END)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	@rm -f $(NAME)

.PHONY: re
re: fclean all

-include $(DEPS)
