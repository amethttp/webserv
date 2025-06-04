#pragma once

#include <string>

class Request
{
private:
	std::string buffer_;
public:
	std::string getBuffer();
	void eraseBuffer(int bytesToErase);
	void appendBuffer(char *buffer);
};
