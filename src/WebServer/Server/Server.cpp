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
