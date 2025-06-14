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
	static const std::string tchars;

	method_t method_;
	std::string target_;
	std::string httpVersion_;
	std::map<std::string, std::string> headers_;
	std::string body_;
	std::string buffer_;
	bool complete_;

	method_t getHTTPMethod(const std::string &method);
	bool isValidHeaderKey(const std::string &key);
	bool isValidHeaderValue(const std::string &value);
	bool isValidHeader(const std::string &key, const std::string &value);
	bool shouldWaitForData(const std::string &str);
	bool tryParseRequestLine(const std::string &string);
	bool tryParseHeaders(std::vector<std::string> &headers);
	bool tryParseFullBody();
	bool tryParseChunkedBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd);
	bool tryParseBody(std::vector<std::string> &headers);
	
public:
	Request();
	~Request();

	std::string getBuffer();
	void clear();
	void appendBuffer(char *buffer);

	bool hasFullHeaders();
	bool tryParseFromBuffer();

	method_t getMethod();
	std::string getTarget();
	std::map<std::string, std::string>  getHeaders();

	friend std::ostream &operator<<(std::ostream &stream, Request &request);
};
