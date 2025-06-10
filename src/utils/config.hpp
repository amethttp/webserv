#pragma once
#include <string>

typedef struct return_s
{
	int code;
	std::string path;
} return_t;

typedef struct error_page_s
{
	int code;
	std::string page;
} error_page_t;