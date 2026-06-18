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
CONFIG_DIR = src/config
CORE_DIR = src/core
ERRORS_DIR = src/errors
HTTP_DIR = src/http
NET_DIR = src/net
UTILS_DIR = src/utils
CGI_DIR = src/cgi

# Files
SRCS = 	\
		$(CGI_DIR)/CgiHandler.cpp \
		$(CGI_DIR)/CgiResponseParser.cpp \
		$(CONFIG_DIR)/Config.cpp \
		$(CONFIG_DIR)/ConfigParser.cpp \
		$(CONFIG_DIR)/ParserUtils.cpp \
		$(CONFIG_DIR)/LocationBlock.cpp \
		$(CONFIG_DIR)/ServerBlock.cpp \
		$(CORE_DIR)/ServerManager.cpp \
		$(ERRORS_DIR)/ErrorCode.cpp \
		$(ERRORS_DIR)/ErrorPageGenerator.cpp \
		$(HTTP_DIR)/AutoindexHandler.cpp \
		$(HTTP_DIR)/DeleteHandler.cpp \
		$(HTTP_DIR)/Dispatcher.cpp \
		$(HTTP_DIR)/HttpRequest.cpp \
		$(HTTP_DIR)/HttpRequestParser.cpp \
		$(HTTP_DIR)/HttpResponse.cpp \
		$(HTTP_DIR)/HttpStatus.cpp \
		$(HTTP_DIR)/MimeTypes.cpp \
		$(HTTP_DIR)/RequestRouter.cpp \
		$(HTTP_DIR)/StaticFileHandler.cpp \
		$(HTTP_DIR)/UploadHandler.cpp \
		$(NET_DIR)/ClientConnection.cpp \
		$(NET_DIR)/Fd.cpp \
		$(NET_DIR)/LookupTable.cpp \
		$(NET_DIR)/Poller.cpp \
		$(NET_DIR)/TcpListener.cpp \
		$(UTILS_DIR)/Checker.cpp \
		$(UTILS_DIR)/Convertor.cpp \
		$(UTILS_DIR)/Logger.cpp \
		main.cpp

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(YELLOW)Linking $(NAME)...$(END)\n"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)Webserv starter is ready!$(END)\n"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "$(BLUE)Compiling $<...$(END)\n"
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
