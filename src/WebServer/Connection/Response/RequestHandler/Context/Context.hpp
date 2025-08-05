#pragma once

#include <string>
#include "utils/http/http.hpp"
#include "WebServer/Connection/Request/Request.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Server/Location/Location.hpp"

class Context
{
private:
	t_method method_;
	const t_Request *request_;
	const Location *location_;
	std::string uploadPath_;
	std::string targetPath_;
	t_connection connectionMode_;

	void checkRequestHeaders();
	void routeTarget();
	void fitMethod();
public:
	Context();
	Context(const t_Request *rq, const Location *lc, const Server *sv);
	~Context();

	void init(const std::vector<Server *> &servers, const t_Request &request);

	t_method getMethod() const;
	const t_Request &getRequest() const;
	const Location &getLocation() const;
	t_return getReturn() const;
	std::string getTargetPath() const;
	std::string getUploadPath() const;
	t_connection getConnectionMode() const;
};
