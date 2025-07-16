#pragma once

#include <string>
#include <ctime>
#include "WebServer/Client/Response/Response.hpp"
#include "WebServer/Client/RequestHandler/Result/HandlingResult.hpp"

class ResponseFactory
{
private:
	static void setStatusLine(t_httpCode code, t_Response &response);
	static void setResponseBody(t_Body &body, t_Response &response);
	static void setDefaultHeaders(t_connection mode, t_Response &response);
	static void addResultHeaders(HeaderCollection &resultHeaders, HeaderCollection &responseHeaders);

public:
	static t_Response create(HandlingResult &res);
	static t_Response create(t_httpCode code, t_connection mode);
};
