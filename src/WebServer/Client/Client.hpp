#pragma once

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
	Request request_;
	Response response_;

public:
	Client();
	~Client();

	int getId();
	fd_t getFd();
	void setFd(fd_t fd);
	std::string getStringifiedResponse();

	void eraseResponse(int bytesToErase);
};
