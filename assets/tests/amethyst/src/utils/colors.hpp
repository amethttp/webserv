#pragma once

#include <cstdio>
#include <unistd.h>

#define GET_COLOR(color) (isatty(fileno(stdout)) ? color : "")

#define RESET		GET_COLOR("\033[0m")
#define RED			GET_COLOR("\033[0;31m")
#define RED_BOLD	GET_COLOR("\033[1;31m")
#define GREEN		GET_COLOR("\033[0;32m")
#define GREEN_BOLD	GET_COLOR("\033[1;32m")
#define YELLOW		GET_COLOR("\033[0;33m")
#define YELLOW_BOLD	GET_COLOR("\033[1;33m")
#define PINK		GET_COLOR("\033[0;35m")
#define CYAN		GET_COLOR("\033[0;36m")
#define CYAN_BOLD	GET_COLOR("\033[1;36m")
#define GREY		GET_COLOR("\033[38;5;251m")
#define WHITE		GET_COLOR("\033[0;39m")
#define WHITE_BOLD	GET_COLOR("\033[1;39m")