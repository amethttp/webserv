#pragma once

#include "utils/fd.hpp"
#include "Request/Request.hpp"
#include "Response/Response.hpp"

class Client
{
private:
public:
	Client();
	~Client();

	fd_t fd_;
	Request request_;
	Response response_;
};
