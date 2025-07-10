#pragma once

#include "HeaderCollection/HeaderCollection.hpp"
#include "RequestLineParams/RequestLineParams.hpp"
#include <string>

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	HeaderCollection headers;
	std::string body;
} Request_t;
