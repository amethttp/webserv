#pragma once

#include <string>

typedef enum method_e
{
	GET,
	POST,
	DELETE,
	NOT_IMPLEMENTED
} method_t;

method_t getHttpMethodFromString(const std::string &requestMethodString);
