#pragma once

#include <map>
#include <string>
#include "utils/HTTP/http.hpp"
#include "RequestLineParams/RequestLineParams.hpp"

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	std::map<std::string, std::string> headers;
	std::string body;
} Request_t;
