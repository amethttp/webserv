#pragma once

#include <string>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "../Request/Request.hpp"

#include "../../Server/Server.hpp"
#include "../../Server/Location/Location.hpp"

class Response
{
private:
	static std::map<httpError_t, std::string> errorDict_;
	static std::map<std::string, std::string> extensionTypesDict_;

	bool connection_;

	std::string buffer_;

	StatusLine statusLine_;
	std::map<std::string, std::string> headers_;
	std::string body_;

	std::string getMIME(std::string &target);
	void setStatusLine(httpError_t code);
	void setResponseHeaders();
	void setRepresentationHeaders(std::string &target);
	
	void generateResponse(httpError_t code);

	void checkRequestHeaders(Request &request);

	bool methodGet(std::string &path);
	bool methodPost(std::string &path);
	bool methodDelete(std::string &path);
	bool executeMethod(method_t method, std::string &path);
public:
	Response();
	~Response();
	std::string toString();

	void setBuffer(const std::string &buffer);

	std::string getBuffer();
	std::string getBody();

	void eraseBuffer(size_t bytesToErase);

	bool tryBuild(Request &request);
};
