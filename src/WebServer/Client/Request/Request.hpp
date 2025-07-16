#pragma once

#include "HeaderCollection/HeaderCollection.hpp"
#include "RequestLineParams/RequestLineParams.hpp"
#include "Body/Body.hpp"

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	HeaderCollection headers;
	Body body;
} Request_t;
