#pragma once

#include "RequestLineParams/RequestLineParams.hpp"
#include <map>
#include <string>

typedef std::map<std::string, std::string> headers_t;

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	headers_t headers;
	std::string body;
} Request_t;
