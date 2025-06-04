#include "Request.hpp"
#include <iostream>

bool Request::parseRequestLine()
{
	return true;
}

std::string Request::getBuffer()
{
	return this->buffer_;
}

void Request::eraseBuffer(int bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}

void Request::appendBuffer(char *buffer)
{
	this->buffer_ += buffer;
}

bool Request::hasFullHeaders()
{
	return this->buffer_.find(HEADERS_DELIM) != std::string::npos;
}

std::vector<std::string> Request::split(const std::string &input, const std::string &del)
{
	std::vector<std::string> result;
	size_t pos = 0;
	size_t next = 0;
	size_t delimLen = del.length();

	next = input.find(del, pos);
	while (next != std::string::npos)
	{
		result.push_back(input.substr(pos, next - pos));
		pos = next + delimLen;
		next = input.find(del, pos);
	}

	result.push_back(input.substr(pos));
	return result;
}

bool Request::parseFromBuffer()
{
	std::vector<std::string> bufferLines = split(this->buffer_, HEADER_LINE_SEP);

	std::cout << "Vector length: " << bufferLines.size() << std::endl;
	for (std::vector<std::string>::iterator it = bufferLines.begin(); it < bufferLines.end(); ++it)
	{
		std::cout << "Line: " << *it << std::endl;
	}

	return true;
}
