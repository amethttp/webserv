#pragma once

#include <string>
#include <vector>
#include <map>
#include "Session/Session.hpp"
#include "Location/Location.hpp"
#include "WebServer/Connection/Request/Request.hpp"

class Server
{
private:
	std::vector<Location *> locations_;
	std::set<int> ports_;
	std::map<int, Session *> sessions_;
	std::vector<std::string> names_;
	std::string uploadPath_;

public:
	Server();
	~Server();

	bool matchesName(std::string &match);
	Location *matchLocation(t_Request request);

	std::set<int> getPorts();
	void setPorts(const std::set<int> &ports);

	std::vector<Location *> getLocations();
	void setLocations(const std::vector<Location *> &locations);

	std::vector<std::string> getNames();
	void setNames(const std::vector<std::string> &names);

	std::string getUploadPath();
	void setUploadPath(const std::string &path);

	void setDefaults();

	friend std::ostream &operator<<(std::ostream &stream, const Server &server);

	std::map<int, Session *> getSessions();
	Session *getSession(int);
	void pushSession(Session *session);
};
