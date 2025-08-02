#pragma once

#include <ctime>
#include <string>
#include <stdint.h>
#include "utils/fd.hpp"
#include "Response/Response.hpp"
#include "Request/Request.hpp"
#include "Response/ResponseFactory/ResponseFactory.hpp"
#include "Response/RequestHandler/RequestHandler.hpp"
#include "Response/RequestHandler/RequestExecutor/RequestExecutor.hpp"
#include "WebServer/Client/Response/RequestHandler/Result/HandlingResult.hpp"

#define DISCONNECTED "disconnected"
#define TIMED_OUT "timed out"

class Client
{
private:
	static uint32_t idCounter_;
	uint32_t id_;
	fd_t fd_;
	time_t lastReceivedPacket_;
	t_Request request_;
	t_Response response_;

	static std::map<t_httpCode, std::string> errorDict_;
	static std::map<std::string, std::string> extensionTypesDict_;

public:
	Client();
	~Client();

	int getId();
	fd_t getFd();
	time_t getLastReceivedPacket() const;
	t_httpCode getResponseStatus() const;
	std::string getResponseBuffer() const;
	const t_Request &getRequest() const;
	const std::string &getRequestBuffer() const;
	void clearRequestBuffer();

	void setFd(fd_t fd);

	bool hasPendingRequest();
	void updateLastReceivedPacket(char *buffer);
	void appendToRequestBuffer(const char *toAppend);
	void eraseResponse(size_t bytesToErase);
	bool shouldClose();

	void setRequest(t_Request request);
	void buildResponse(t_HandlingResult &result);
	void buildResponse(t_httpCode code, t_connection mode);

	static std::string getHttpErrorMsg(t_httpCode code);
	static std::string getExtensionType(std::string extension);
};
