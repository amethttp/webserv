#pragma once

#include <string>
#include "utils/http.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Server/Location/Location.hpp"

class Context
{
public:
	t_method method_;
	Request &request_;
	Location &location_;
	std::string uploadPath_;
	std::string targetPath_;
	t_connection connectionMode_;

	void checkRequestHeaders();
	void routeTarget();
	void fitMethod();
public:
	Context(Request &request, Location &location, Server &server);
	~Context();

	t_method getMethod() const;
	t_return getReturn() const;
	std::string getTargetPath() const;
	t_connection getConnectionMode() const;
};
