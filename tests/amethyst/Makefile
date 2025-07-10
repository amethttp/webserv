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
NAME = amethtdd

#----COMPILER----#
CC = c++
CCFLAGS += -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g # -Ofast


#----DIRS----#
BIN_DIR = bin/
SRC = src/
PROJ_SRC=../../src/
SUITES=suites/
SUITES_DIRECTORIES=$(shell find $(SUITES)* -type d 2>/dev/null | tr '\n' ':')
INCLUDES = -I$(SRC) -I$(PROJ_SRC)

#----VARS----#
SUITES_FILES=$(shell find $(SUITES)* -type f -name "*.cpp" 2>/dev/null | xargs basename -a 2>/dev/null | tr '\n' ' ')


#----VPATH----#
vpath %.cpp $(SRC):\
            $(SRC)test:\
            $(SRC)test/TestFailure:\
            $(SUITES):\
            $(SUITES_DIRECTORIES)\
            $(PROJ_SRC)WebServer/Client/Request/HeaderCollection/\
            $(PROJ_SRC)WebServer/Client/Request/HeaderCollection/Header\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestValidator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestValidator/HostHeaderValidator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestValidator/ContentLengthHeaderValidator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestValidator/TransferEncodingHeaderValidator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestValidator/ConnectionHeaderValidator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestProcesser\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestTargetSeparator\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestPctDecoder\
            $(PROJ_SRC)WebServer/Client/Request/RequestFactory/RequestTargetNormalizer\
            $(PROJ_SRC)WebServer/Client/Request/RequestParser/\
            $(PROJ_SRC)WebServer/Client/Request/RequestTokenizer/\
            $(PROJ_SRC)WebServer/Client/Request/RequestTokenizer/RequestToken/\
            $(PROJ_SRC)utils/headers/\
            $(PROJ_SRC)utils/HTTP/\
            $(PROJ_SRC)utils/string/\
            $(PROJ_SRC)utils/numeric/\
            $(PROJ_SRC)utils/Result/\
			

#----SHARED----#
SRCS = main.cpp \
       test.cpp \
       TestFailure.cpp \
       $(SUITES_FILES) \
       HeaderCollection.cpp \
       Header.cpp \
       RequestFactory.cpp \
       RequestValidator.cpp \
       HostHeaderValidator.cpp \
       ContentLengthHeaderValidator.cpp \
       TransferEncodingHeaderValidator.cpp \
       ConnectionHeaderValidator.cpp \
       RequestProcesser.cpp \
       RequestTargetSeparator.cpp \
       RequestPctDecoder.cpp \
       RequestTargetNormalizer.cpp \
       RequestParser.cpp \
       RequestTokenizer.cpp \
       RequestToken.cpp \
       headers.cpp \
       http.cpp \
       string.cpp \
       numeric.cpp \
       SimpleResult.cpp \

OBJS = $(SRCS:%.cpp=$(BIN_DIR)%.o)
DEPS = $(OBJS:%.o=%.d)



#----- R U L E S -----#

all:
	@mkdir -p $(SUITES)
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

.PHONY: all \
		clean \
		fclean \
		re

-include $(DEPS)
-include $(MDEPS)
-include $(BDEPS)
.SILENT:
