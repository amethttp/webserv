#pragma once

#include <string>
#include "utils/http.hpp"
#include "Request/Request.hpp"

class Response
{
private:
	std::string buffer_;

	method_t method_;
	std::string target_;
	std::string httpVersion_;
	std::map<std::string, std::string> headers_;
	std::string body_;
public:
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(int bytesToErase);

	bool methodGet();
	bool methodPost();
	bool methodDelete();
	bool tryBuild(Request &request);
};
