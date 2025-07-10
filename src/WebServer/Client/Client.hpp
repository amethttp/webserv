#pragma once

#include <ctime>
#include <string>
#include <stdint.h>
#include "utils/fd.hpp"
#include "Request/Request.hpp"
#include "Response/Response.hpp"
#include "utils/Result/Result.hpp"
#include "Result/ExecutionResult.hpp"
#include "RequestExecutor/RequestExecutor.hpp"
#include "RouteMatchers/Server/ServerMatcher.hpp"
#include "RouteMatchers/Location/LocationMatcher.hpp"

#define DISCONNECTED "disconnected"
#define TIMED_OUT "timed out"

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
	Request getRequest();
	t_httpCode getResponseStatus();

	void setFd(fd_t fd);
	void setResponseBuffer(const std::string &stringResponse);

	void updateLastReceivedPacket();
	bool hasFullRequestHeaders();
	void buildRequest();
	void appendRequest(char *request);
	bool hasPendingRequest();
	void clearRequest();
	void eraseResponse(size_t bytesToErase);
	bool shouldClose();

	void executeRequest(std::vector<Server> &servers);
	void buildResponse(std::vector<Server> &servers);
	void buildResponse(t_httpCode code, t_connection mode);
};
