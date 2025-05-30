#pragma once

#include <string>
#include <vector>
#include "Session/Session.hpp"
#include "Location/Location.hpp"

class Server
{
private:
	std::vector<Location> locations_;
	std::vector<int> ports_;
	std::vector<Session> sessions_;
	std::vector<std::string> names_;
	std::string uploadPath_;

public:
};
