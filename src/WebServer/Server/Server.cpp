#include "Server.hpp"

Server::Server()
{
}

Server::~Server()
{
}

bool Server::matchesName(std::string &match)
{
	for (std::vector<std::string>::iterator name = this->names_.begin(); name != this->names_.end(); ++name)
	{
		if (*name == match)
			return true;
	}

	return false;
}

std::vector<int> Server::getPorts()
{
	return this->ports_;
}

void Server::setPorts(std::vector<int> &ports)
{
	this->ports_ = ports;
}

std::vector<Location *> Server::getLocations()
{
	return this->locations_;
}

void Server::setLocations(std::vector<Location *> &locations)
{
	this->locations_ = locations;
}

std::vector<std::string> Server::getNames()
{
    return this->names_;
}

void Server::setNames(std::vector<std::string> &names)
{
	this->names_ = names;
}

std::string Server::getUploadPath()
{
    return this->uploadPath_;
}

void Server::setUploadPath(std::string path)
{
	this->uploadPath_ = path;
}
