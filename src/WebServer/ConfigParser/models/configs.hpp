#pragma once
#include <string>
#include "WebServer/WebServer.hpp"

typedef struct s_configs
{
	std::string *path;
	WebServer *ws;
	std::string fileContent;
} t_configs;