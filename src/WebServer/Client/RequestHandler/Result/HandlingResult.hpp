#pragma once

#include <string>
#include <map>
#include "utils/http.hpp"
#include "WebServer/Client/Response/Response.hpp"
#include "utils/HeaderCollection/HeaderCollection.hpp"

class HandlingResult
{
public:
	t_httpCode code_;
	t_connection mode_;
	HeaderCollection tempHeaders_;
	t_Body tempBody_;
	bool isCGI_;

public:
	HandlingResult();
	~HandlingResult();
};
