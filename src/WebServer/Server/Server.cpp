#include "Server.hpp"

Server::Server() : Config()
{
}

void Server::addLocation(Location &location)
{
	this->locations_.push_back(location);
}