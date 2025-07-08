#pragma once

#include "RequestLineParams/RequestLineParams.hpp"
#include <map>
#include <string>
#include <vector>

typedef std::vector<std::string> headerValue_t;
typedef std::map<std::string, headerValue_t> headers_t;

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	headers_t headers;
	std::string body;
} Request_t;
