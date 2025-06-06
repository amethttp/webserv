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

std::string Client::getStringifiedResponse()
{
	return this->response_.toString();
}

std::string Client::getStringifiedRequest()
{
	return this->request_.getBuffer();
}

void Client::setFd(fd_t fd)
{
	this->fd_ = fd;
}

void Client::setResponseBuffer(const std::string &stringResponse)
{
	this->response_.setBuffer(stringResponse);
}

bool Client::hasFullRequestHeaders()
{
	return this->request_.hasFullHeaders();
}

bool Client::parseRequest()
{
	return this->request_.parseFromBuffer();
}

void Client::appendRequest(char *request)
{
	this->request_.appendBuffer(request);
}

void Client::eraseRequest(int bytesToErase)
{
	this->request_.eraseBuffer(bytesToErase);
}

void Client::eraseResponse(int bytesToErase)
{
	this->response_.eraseBuffer(bytesToErase);
}
