#pragma once

#include "Body/Body.hpp"
#include "RequestLine/RequestLine.hpp"
#include "utils/HeaderCollection/HeaderCollection.hpp"

typedef struct s_Request
{
	RequestLine requestLine;
	HeaderCollection headers;
	Body body;

	std::string buffer;
} t_Request;
