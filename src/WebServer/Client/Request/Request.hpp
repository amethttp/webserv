#pragma once

#include "HeaderCollection/HeaderCollection.hpp"
#include "RequestLineParams/RequestLineParams.hpp"
#include "Body/Body.hpp"
#include <string>

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	HeaderCollection headers;
	Body bodyNew;
	std::string body;
} Request_t;
