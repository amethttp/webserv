#pragma once

#include <sys/epoll.h>
#include "fd.hpp"

#define EVENT_BUFFER_SIZE 64

typedef struct s_epoll
{
	fd_t fd;
	epoll_event eventConfig;
	epoll_event eventBuffer[EVENT_BUFFER_SIZE];
} t_epoll;
