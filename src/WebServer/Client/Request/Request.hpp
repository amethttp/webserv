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

	t_method method_;
	std::string target_;
	std::string httpVersion_;
	std::map<std::string, std::string> headers_;
	std::string body_;
	std::string buffer_;
	bool complete_;

	t_method getHTTPMethod(const std::string &method);
	bool isValidHeaderKey(const std::string &key);
	bool isValidHeaderValue(const std::string &value);
	bool isValidHeader(const std::string &key, const std::string &value);
	bool shouldWaitForData(const std::string &str);
	bool checkValidHTTPProtocol();
	bool tryParseRequestLine(const std::string &string);
	bool tryParseHeaders(std::vector<std::string> &headers);
	bool tryParseFullBody();
	bool tryParseChunkedBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd);
	bool tryParseBody(std::vector<std::string> &headers);
	
public:
	Request();
	~Request();

	void clear();
	void appendBuffer(char *buffer);

	bool hasFullHeaders();
	bool tryParseFromBuffer();

	t_method getMethod();
	std::string getBuffer();
	std::string getBody();
	std::string getTarget();
	std::string getHTTPVersion();
	std::map<std::string, std::string>  getHeaders();
	bool isComplete();

	void setComplete(bool status);
	void setMethod(t_method method);

	friend std::ostream &operator<<(std::ostream &stream, Request &request);
};
