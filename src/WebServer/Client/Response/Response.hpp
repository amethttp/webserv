#pragma once

#include <string>

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

	void methodGet();
	void methodPOST();
	void methodDELETE();
	bool tryBuild(Request &request);
};
