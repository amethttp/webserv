#pragma once

#include <string>

class Response
{
private:
	std::string buffer_;
public:
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(int bytesToErase);
};
