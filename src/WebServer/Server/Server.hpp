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
	Server();
	~Server();

	std::vector<int> getPorts();
	void setPorts(std::vector<int> &ports);

	std::vector<Location> getLocations();
	void setLocations(std::vector<Location> &locations);

	std::vector<std::string> getNames();
	void setNames(std::vector<std::string> &names);

	std::string getUploadPath();
	void setUploadPath(std::string path);
};
