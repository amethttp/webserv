#pragma once

#include "HeaderCollection/HeaderCollection.hpp"
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
	HeaderCollection headersNew;
	std::string body;
} Request_t;
