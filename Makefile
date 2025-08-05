#----COLORS----#
DEF_COLOR = \033[0m
WHITE_BOLD = \033[1;39m
BLACK = \033[1;30m
RED = \033[1;31m
RED_REGULAR = \033[0;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
PINK = \033[1;35m
CYAN = \033[1;36m

#----OS COMPATIBILITY----#
ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX=1
		export LINUX=1
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D DARWIN=1
		export DARWIN=1
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

#----NAME----#
NAME = webserv

#----COMPILER----#
CC = c++
CCFLAGS += -std=c++98 -Wall -Wextra -Werror -Wpedantic -fsanitize=address -g -w # -Ofast


#----DIRS----#
BIN_DIR = bin/
SRC = src/
INCLUDES = -I$(SRC)

#----VPATH----#
vpath %.cpp $(SRC): \
			$(SRC)WebServer/ConfigParser : \
			$(SRC)WebServer/ConfigParser/helpers/DirectiveRegistry/ : \
			$(SRC)utils: \
			$(SRC)WebServer/Connection/Request/Body : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestBodyFramingVerifier : \
			$(SRC)WebServer/Connection/Request/RequestFactory : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestPathNormalizer : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestPctDecoder : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestProcesser : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestValidator/ConnectionHeaderValidator : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestValidator/ContentLengthHeaderValidator : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestValidator/HostHeaderValidator : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestValidator : \
			$(SRC)WebServer/Connection/Request/RequestFactory/RequestValidator/TransferEncodingHeaderValidator : \
			$(SRC)WebServer/Connection/Request/RequestLine : \
			$(SRC)WebServer/Connection/Request/RequestLine/Target : \
			$(SRC)WebServer/Connection/Request/RequestParser : \
			$(SRC)WebServer/Connection/Request/RequestTokenizer/RequestToken : \
			$(SRC)WebServer/Connection/Request/RequestTokenizer : \
			$(SRC)WebServer/Connection/Response/ResponseFactory : \
			$(SRC)WebServer/Connection/Response/StatusLine : \
			$(SRC)WebServer/Connection/Response/RequestHandler/Context : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods/Factory : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods/Implemented/DELETE : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods/Implemented/GET : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods/Implemented/POST : \
			$(SRC)WebServer/Connection/Response/RequestHandler/RequestExecutor : \
			$(SRC)WebServer/Connection/Response/RequestHandler/Result : \
			$(SRC)WebServer/Connection/Response/RequestHandler : \
			$(SRC)WebServer/Connection : \
			$(SRC)WebServer/Server/Location : \
			$(SRC)WebServer/Server/Session : \
			$(SRC)WebServer/Server : \
			$(SRC)WebServer : \
			$(SRC)utils/HeaderCollection/Header : \
			$(SRC)utils/HeaderCollection : \
			$(SRC)utils/cgi : \
			$(SRC)utils/exceptions : \
			$(SRC)utils/fileHandler : \
			$(SRC)utils/numeric : \
			$(SRC)utils/string : \
			$(SRC)utils/Result : \
			$(SRC)utils/http :

#----SHARED----#
SRCS = Body.cpp \
		RequestBodyFramingVerifier.cpp \
		RequestFactory.cpp \
		RequestPathNormalizer.cpp \
		RequestPctDecoder.cpp \
		RequestProcesser.cpp \
		ConnectionHeaderValidator.cpp \
		ContentLengthHeaderValidator.cpp \
		HostHeaderValidator.cpp \
		RequestValidator.cpp \
		TransferEncodingHeaderValidator.cpp \
		RequestLine.cpp \
		Target.cpp \
		RequestParser.cpp \
		RequestToken.cpp \
		RequestTokenizer.cpp \
		ResponseFactory.cpp \
		StatusLine.cpp \
		Context.cpp \
		AMethod.cpp \
		MethodFactory.cpp \
		Delete.cpp \
		Get.cpp \
		Post.cpp \
		RequestExecutor.cpp \
		RequestHandler.cpp \
		Connection.cpp \
		Location.cpp \
		Session.cpp \
		Server.cpp \
		WebServer.cpp \
		Header.cpp \
		HeaderCollection.cpp \
		cgi.cpp \
		Exceptions.cpp \
		files.cpp \
		numeric.cpp \
		string.cpp \
		SimpleResult.cpp \
		webserv.cpp \
		ConfigParser.cpp \
		DirectiveRegistry.cpp \
		http.cpp


OBJS = $(SRCS:%.cpp=$(BIN_DIR)%.o)
DEPS = $(OBJS:%.o=%.d)

AMETHTDD=./tests/amethyst/amethtdd

#----- R U L E S -----#

all:
	@$(MAKE) --no-print-directory $(NAME)

$(NAME): $(OBJS)
	@printf "$(BLUE)Linking objects and creating program...$(DEF_COLOR)\n"
	$(CC) $(CCFLAGS) $(OBJS) $(LIBRARIES) $(LIBRARIES_DEPS) -o $(NAME)
	@echo "$(GREEN)[✓] $(PINK)$(NAME)$(GREEN) created!!!$(DEF_COLOR)"

$(BIN_DIR)%.o: %.cpp Makefile
	@printf "$(CYAN)Compiling: $(PINK)$(notdir $<)...$(DEF_COLOR)\n"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CCFLAGS) $(INCLUDES) -MMD -c $< -o $@

clean:
	@rm -rf $(BIN_DIR)
	@echo "$(RED)Binaries deleted$(DEF_COLOR)\n"

fclean: clean
	@rm -rf $(NAME)
	@echo "$(RED)Executable deleted$(DEF_COLOR)\n"

re: fclean all

bonus:
	$(MAKE) --no-print-directory all BONUS=1

b: bonus

bonusre: fclean bonus

bre: bonusre

test:
	cd tests/subject; ./ubuntu_tester "http://localhost:8080"

tester:
	$(MAKE) --no-print-directory -C tests/amethyst && $(AMETHTDD)

testercl:
	$(MAKE) --no-print-directory -C tests/amethyst clean

testerfcl:
	$(MAKE) --no-print-directory -C tests/amethyst fclean

testerre:
	$(MAKE) --no-print-directory -C tests/amethyst re && $(AMETHTDD)

.PHONY: all \
		clean \
		fclean \
		re \
		bonus \
		b \
		bonusre \
		bre \
		test \
		# min-test \
		# test-clean

-include $(DEPS)
-include $(MDEPS)
-include $(BDEPS)
.SILENT:
