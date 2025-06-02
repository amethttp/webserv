#include "Client.hpp"

uint32_t Client::idCounter_ = 1;

Client::Client()
{
	this->id_ = Client::idCounter_;
	Client::idCounter_++;
}

Client::~Client()
{
}

int Client::getId()
{
	return this->id_;
}

fd_t Client::getFd()
{
	return this->fd_;
}

void Client::setFd(fd_t fd)
{
	this->fd_ = fd;
}

std::string Client::getStringifiedResponse()
{
	return this->response_.toString();
}
