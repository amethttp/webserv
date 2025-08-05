#pragma once

#include <string>
#include <map>
#include "utils/http/http.hpp"
#include "WebServer/Connection/Response/Response.hpp"
#include "utils/HeaderCollection/HeaderCollection.hpp"

typedef struct s_HandlingResult
{
	t_httpCode code_;
	t_connection mode_;
	HeaderCollection tempHeaders_;
	t_Body tempBody_;
	bool isCGI_;

	s_HandlingResult()
	{
		code_ = 0;
		isCGI_ = false;
		mode_ = C_KEEP_ALIVE;
	}
} t_HandlingResult;