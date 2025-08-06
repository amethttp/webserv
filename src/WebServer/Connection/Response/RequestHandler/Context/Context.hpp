#pragma once

#include <string>
#include "utils/http/http.hpp"
#include "WebServer/Connection/Request/Request.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Server/Location/Location.hpp"

#define NO_SERVER_MATCH "Recoverable Exception: <Couldn't match server>"
#define NO_LOCATION_MATCH "Recoverable Exception: <Couldn't match location>"

class Context
{
private:
	t_method method_;
	const t_Request *request_;
	const Location *location_;
	Server *server_;
	std::string uploadPath_;
	std::string targetPath_;
	t_connection connectionMode_;

	void checkRequestHeaders();
	void routeTarget();
	void fitMethod();
public:
	Context();
	Context(const t_Request *rq, const Location *lc, Server *sv);
	~Context();

	void init(const std::vector<Server *> &servers, const t_Request &request);

	t_method getMethod() const;
	const t_Request &getRequest() const;
	const Location &getLocation() const;
	const Server &getServer() const;
	t_return getReturn() const;
	std::string getTargetPath() const;
	std::string getUploadPath() const;
	t_connection getConnectionMode() const;

	void setTargetPath(const std::string &path);

	void pushServerSession(Session *session);
};
