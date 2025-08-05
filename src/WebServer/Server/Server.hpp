#pragma once

#include <string>
#include <vector>
#include "Session/Session.hpp"
#include "Location/Location.hpp"
#include "WebServer/Connection/Request/Request.hpp"

class Server
{
private:
	std::vector<Location *> locations_;
	std::set<int> ports_;
	std::vector<Session> sessions_;
	std::vector<std::string> names_;
	std::string uploadPath_;

public:
	Server();
	~Server();

	bool matchesName(std::string &match);
	Location *matchLocation(std::string targetRoute);

	std::set<int> getPorts() const;
	void setPorts(const std::set<int> &ports);

	std::vector<Location *> getLocations() const;
	void setLocations(const std::vector<Location *> &locations);

	std::vector<std::string> getNames() const;
	void setNames(const std::vector<std::string> &names);

	std::string getUploadPath() const;
	void setUploadPath(const std::string &path);
	
	void setDefaults();
	
	friend std::ostream &operator<<(std::ostream &stream, const Server &server);
};
