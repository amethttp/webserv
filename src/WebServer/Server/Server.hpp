#pragma once

#include <string>
#include <vector>
#include "Session/Session.hpp"
#include "Location/Location.hpp"
#include "WebServer/Config/Config.hpp"

class Server: public Config
{
private:
	std::vector<Location> locations_;
	std::vector<int> ports_;
	std::vector<Session> sessions_;
	std::vector<std::string> names_;
	std::string uploadPath_;

public:
	Server();
	void addLocation(Location &location);
};
