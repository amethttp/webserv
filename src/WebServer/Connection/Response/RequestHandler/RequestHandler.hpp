#pragma once

#include "Context/Context.hpp"
#include "Result/HandlingResult.hpp"
#include "WebServer/Connection/Response/Response.hpp"

class RequestHandler
{
private:

public:
	static t_HandlingResult handleRequest(const Context &ctx);
};
