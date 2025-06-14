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

	bool endConnection_;

	std::string buffer_;

	method_t method_;
	std::string targetPath_;

	StatusLine statusLine_;
	std::map<std::string, std::string> headers_;
	std::string body_;

	std::string getMIME(std::string &target);
	void setStatusLine(httpError_t code);
	void setResponseHeaders();
	void setRepresentationHeaders(std::string &target);
	void setParameters(Request &request);
	
	void generateResponse(httpError_t code);

	void checkRequestHeaders(Request &request);

	void methodGet();
	void methodPost();
	void methodDelete();
	void executeRequest();
public:
	Response();
	~Response();
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(size_t bytesToErase);

	std::string getBuffer();
	std::string getBody();
	bool getConnection();


	void build(Request &request);
};
