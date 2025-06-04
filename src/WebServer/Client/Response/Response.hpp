#pragma once

#include <string>

class Response
{
private:
	std::string buffer_;
public:
	std::string toString();

	void eraseBuffer(int bytesToErase);
};
