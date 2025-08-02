#pragma once

#include "Context/Context.hpp"
#include "Result/HandlingResult.hpp"
#include "WebServer/Client/Response/Response.hpp"

class RequestHandler
{
private:
	static void handleExecution(Context &ctx, t_HandlingResult &res);
	static void handleReturnDirective(Context &ctx, t_HandlingResult &res);

public:
	static t_HandlingResult handleRequest(const t_Request&request, Location &location, Server &server);
};
