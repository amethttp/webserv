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
CCFLAGS += -std=c++98 -fsanitize=address -g # -Ofast


#----DIRS----#
BIN_DIR = bin/
SRC = src/
INCLUDES = -I$(SRC)

#----VPATH----#
vpath %.cpp $(SRC): \
            $(SRC)utils: \
            $(SRC)utils/string: \
            $(SRC)utils/numeric: \
            $(SRC)utils/Result: \
            $(SRC)WebServer: \
            $(SRC)WebServer/Client: \
            $(SRC)WebServer/Client/Request: \
            $(SRC)WebServer/Client/Request/RequestFactory: \
            $(SRC)WebServer/Client/Request/RequestParser: \
            $(SRC)WebServer/Client/Response: \
            $(SRC)WebServer/Server: \
            $(SRC)WebServer/Server/Location: \
            $(SRC)WebServer/Server/Session: \


#----SHARED----#
SRCS = webserv.cpp \
        WebServer.cpp \
        Client.cpp \
        Request.cpp \
        Response.cpp \
        Server.cpp \
        Location.cpp \
        Session.cpp \
        string.cpp \
        numeric.cpp \
        SimpleResult.cpp \
        RequestFactory.cpp\
        RequestParser.cpp \

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
