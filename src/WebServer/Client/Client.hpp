#pragma once

#include "utils/fd.hpp"
#include "Request/Request.hpp"
#include "Response/Response.hpp"

class Client
{
private:
	fd_t fd;
	Request request;
	Response response;
};
