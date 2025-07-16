#pragma once

#include "Body/Body.hpp"
#include "RequestLine/RequestLine.hpp"
#include "HeaderCollection/HeaderCollection.hpp"

typedef struct Request_s
{
	RequestLineParams_t requestLine;
	HeaderCollection headers;
	Body body;
} Request_t;
