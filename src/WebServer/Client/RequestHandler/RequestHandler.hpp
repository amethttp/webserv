#pragma once

#include "Context/Context.hpp"
#include "Result/HandlingResult.hpp"
#include "WebServer/Client/Response/Response.hpp"

class RequestHandler
{
private:
	static void handleExecution(Context &ctx, HandlingResult &res);
	static void handleReturnDirective(Context &ctx, HandlingResult &res);

public:
	static HandlingResult handleRequest(Request &request, Location &location, Server &server);
};
