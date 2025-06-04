#pragma once

#include <map>
#include <vector>
#include <string>

#define HEADERS_DELIM "\r\n\r\n"
#define HEADER_LINE_SEP "\r\n"
#define REQUEST_LINE_SEP ' '

class Request
{
private:
	std::map<std::string, std::string> headers_;
	std::string buffer_;

	std::vector<std::string> split(const std::string &input, const std::string &del);
	bool parseRequestLine();

public:
	std::string getBuffer();
	void eraseBuffer(int bytesToErase);
	void appendBuffer(char *buffer);

	bool hasFullHeaders();
	bool parseFromBuffer();
};
