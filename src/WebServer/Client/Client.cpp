#include "Client.hpp"

#include <iostream>

uint32_t Client::idCounter_ = 1;

Client::Client()
{
	this->id_ = Client::idCounter_;
	this->lastReceivedPacket_ = std::time(NULL);
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

time_t Client::getLastReceivedPacket()
{
    return this->lastReceivedPacket_;
}

std::string Client::getStringifiedResponse()
{
	return this->response_.getBuffer();
}

std::string Client::getStringifiedRequest()
{
	return this->request_.getBuffer();
}

Request Client::getRequest()
{
    return this->request_;
}

void Client::setFd(fd_t fd)
{
	this->fd_ = fd;
}

void Client::setResponseBuffer(const std::string &stringResponse)
{
	this->response_.setBuffer(stringResponse);
}

void Client::updateLastReceivedPacket()
{
	this->lastReceivedPacket_ = std::time(NULL);
}

bool Client::hasFullRequestHeaders()
{
	return this->request_.hasFullHeaders();
}

bool Client::tryBuildRequest()
{
	bool result = this->request_.tryParseFromBuffer();
	
	std::cout << this->request_ << std::endl;
	std::cout << "Should build a response: " << (result ? "true" : "false") << std::endl;
	return result;
}

void Client::appendRequest(char *request)
{
	this->request_.appendBuffer(request);
}

void Client::clearRequest()
{
	this->request_.clear();
}

void Client::eraseResponse(size_t bytesToErase)
{
	this->response_.eraseBuffer(bytesToErase);
}

bool Client::tryBuildResponse(Request &Request)
{
    return this->response_.tryBuild(request_);
}
