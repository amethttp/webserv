#pragma once

#include <map>
#include <string>
#include "utils/http.hpp"

typedef struct Request_s
{
	method_t method;
	std::string target;
	std::string httpVersion;
	std::map<std::string, std::string> headers;
	std::string body;
} Request_t;
