#pragma once

#include <ctime>
#include <string>
#include <stdint.h>
#include "utils/fd.hpp"
#include "Request/Request.hpp"
#include "Response/Response.hpp"

class Client
{
private:
	static uint32_t idCounter_;
	uint32_t id_;
	fd_t fd_;
	time_t lastReceivedPacket_;
	Request request_;
	Response response_;

public:
	Client();
	~Client();

	int getId();
	fd_t getFd();
	time_t getLastReceivedPacket();
	std::string getStringifiedResponse();
	std::string getStringifiedRequest();

	void setFd(fd_t fd);
	void setResponseBuffer(const std::string &stringResponse);

	void updateLastReceivedPacket();
	bool hasFullRequestHeaders();
	bool parseRequest();
	void appendRequest(char *request);
	void eraseRequest(int bytesToErase);
	void eraseResponse(int bytesToErase);
};
