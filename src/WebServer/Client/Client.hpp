#pragma once

#include <ctime>
#include <string>
#include <stdint.h>
#include "utils/fd.hpp"
#include "Response/Response.hpp"
#include "Request/Request.hpp"
#include "Response/ResponseFactory/ResponseFactory.hpp"
#include "RequestHandler/RequestHandler.hpp"
#include "RequestHandler/RequestExecutor/RequestExecutor.hpp"
#include "RouteMatchers/Server/ServerMatcher.hpp"
#include "RouteMatchers/Location/LocationMatcher.hpp"
#include "WebServer/Client/RequestHandler/Result/HandlingResult.hpp"

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
	t_Response response_;
	std::string responseBuffer_;

	static std::map<t_httpCode, std::string> errorDict_;
	static std::map<std::string, std::string> extensionTypesDict_;

public:
	Client();
	~Client();

	int getId();
	fd_t getFd();
	time_t getLastReceivedPacket() const;
	std::string getStringifiedResponse();
	std::string getStringifiedRequest();
	Request getRequest() const;
	t_httpCode getResponseStatus() const;
	std::string getResponseBuffer() const;

	void setFd(fd_t fd);
	void updateResponse();

	void updateLastReceivedPacket();
	bool hasFullRequestHeaders();
	void buildRequest();
	void appendRequest(char *request);
	bool hasPendingRequest();
	void clearRequest();
	void eraseResponse(size_t bytesToErase);
	bool shouldClose();

	void buildResponse(std::vector<Server *> &servers);
	void buildResponse(t_httpCode code, t_connection mode);

	static std::string getHttpErrorMsg(t_httpCode code);
	static std::string getExtensionType(std::string extension);
};
