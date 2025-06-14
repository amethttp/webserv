#include "Server.hpp"

Server::Server()
{
}

Server::~Server()
{
}

std::vector<int> Server::getPorts()
{
	return this->ports_;
}

void Server::setPorts(std::vector<int> &ports)
{
	this->ports_ = ports;
}

std::vector<Location> Server::getLocations()
{
	return this->locations_;
}

void Server::setLocations(std::vector<Location> &locations)
{
	this->locations_ = locations;
}
