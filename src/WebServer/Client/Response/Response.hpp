#pragma once

#include <string>
#include "StatusLine/StatusLine.hpp"
#include "utils/HeaderCollection/HeaderCollection.hpp"

typedef struct s_Body
{
	std::string content;
	std::string type;
} t_Body;

typedef struct s_Response
{
	StatusLine statusLine_;
	HeaderCollection headers_;
	t_Body body_;
} t_Response;
