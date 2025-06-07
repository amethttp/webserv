#pragma once

#include <map>
#include <vector>
#include <string>
#include <ostream>
#include "utils/http.hpp"

#define VALID_PROTOCOL "HTTP/1.1"
#define HEADERS_DELIM "\r\n\r\n"
#define HEADER_LINE_SEP "\r\n"
#define REQUEST_LINE_SEP ' '
#define CONTENT_LENGTH "Content-Length"
#define TRANSFER_ENCODING "Transfer-Encoding"

class Request
{
private:
	int method_;
	std::string target_;
	std::string httpVersion_;
	std::map<std::string, std::string> headers_;
	std::string body_;
	std::string buffer_;
	bool complete_;

	int getHTTPMethod(const std::string &method);
	bool tryParseRequestLine(const std::string &string);
	bool tryParseHeaders(std::vector<std::string> &headers);
	bool tryParseFullBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd);
	bool tryParseChunkedBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd);
	bool tryParseBody(std::vector<std::string> &headers);
	
public:
	Request();
	~Request();

	std::string getBuffer();
	void eraseBuffer(int bytesToErase);
	void appendBuffer(char *buffer);

	bool hasFullHeaders();
	bool tryParseFromBuffer();

	friend std::ostream &operator<<(std::ostream &stream, Request &request);
};
