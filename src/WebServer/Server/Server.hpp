#pragma once

#include <string>
#include <vector>
#include "Session/Session.hpp"
#include "Location/Location.hpp"
#include "WebServer/Client/Request/Request.hpp"

class Server
{
private:
	std::vector<Location *> locations_;
	std::vector<int> ports_;
	std::vector<Session> sessions_;
	std::vector<std::string> names_;
	std::string uploadPath_;

public:
	Server();
	~Server();

	bool matchesName(std::string &match);
	Location *matchLocation(t_Request request);

	std::vector<int> getPorts();
	void setPorts(const std::vector<int> &ports);

	std::vector<Location *> getLocations();
	void setLocations(const std::vector<Location *> &locations);

	std::vector<std::string> getNames();
	void setNames(const std::vector<std::string> &names);

	std::string getUploadPath();
	void setUploadPath(const std::string &path);
};
