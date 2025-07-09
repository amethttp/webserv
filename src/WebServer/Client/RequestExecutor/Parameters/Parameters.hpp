#pragma once

#include <string>
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Location/Location.hpp"
#include "utils/http.hpp"

class Parameters
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
	void setParameters();
public:
	Parameters(Request &request, Location &location);
	~Parameters();

	t_method getMethod() const;
	std::string getTargetPath() const;
	t_connection getConnectionMode() const;
};
