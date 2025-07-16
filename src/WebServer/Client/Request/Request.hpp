#pragma once

#include "Body/Body.hpp"
#include "RequestLine/RequestLine.hpp"
#include "HeaderCollection/HeaderCollection.hpp"

typedef struct Request_s
{
	RequestLine requestLineNew;
	RequestLineParams_t requestLine;
	HeaderCollection headers;
	Body body;
} Request_t;
