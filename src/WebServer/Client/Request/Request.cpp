#include "Request.hpp"
#include "utils/string/string.hpp"
#include "utils/numeric/numeric.hpp"
#include <iostream>
#include <algorithm>

Request::Request()
{
	this->complete_ = false;
}

Request::~Request()
{
}

method_t Request::getHTTPMethod(const std::string &method)
{
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	return NOT_ALLOWED;
}

bool Request::tryParseRequestLine(const std::string &string)
{
	std::vector<std::string> splittedLine = split(string, " ");
	if (splittedLine.size() != 3)
		return false;

	this->method_ = getHTTPMethod(splittedLine[0]);
	if (this->method_ == NOT_ALLOWED)
		return false;

	this->target_ = splittedLine[1];

	this->httpVersion_ = splittedLine[2];
	if (this->httpVersion_ != VALID_PROTOCOL)
		return false;

	return true;
}

bool Request::tryParseHeaders(std::vector<std::string> &headers)
{
	for (std::vector<std::string>::iterator it = headers.begin() + 1; it != headers.end() && !it->empty(); ++it)
	{
		std::vector<std::string> splittedHeader = split(*it, ": ");

		if (splittedHeader.size() != 2)
			return false;

		this->headers_[splittedHeader[0]] = splittedHeader[1];
	}

	std::map<std::string, std::string>::iterator contentLengthIt = this->headers_.find(CONTENT_LENGTH);
	std::map<std::string, std::string>::iterator transferEncodingIt = this->headers_.find(TRANSFER_ENCODING);

	if ((this->headers_.find("Host") == this->headers_.end())
		|| (this->method_ == POST && contentLengthIt == this->headers_.end() && transferEncodingIt == this->headers_.end())
		|| (contentLengthIt != this->headers_.end() && transferEncodingIt != this->headers_.end())
		|| (contentLengthIt != this->headers_.end() && !isLong(contentLengthIt->second) && atol(contentLengthIt->second.c_str()) >= 0)
		|| (transferEncodingIt != this->headers_.end() && transferEncodingIt->second != "chunked"))
		return false;

	return true;
}

bool Request::tryParseFullBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd)
{
	std::string tempBuffer = *bodyIt;

	while (bodyIt != bodyEnd)
	{
		tempBuffer += *bodyIt;
		++bodyIt;
	}

	long contentLength = atol(this->headers_.find(CONTENT_LENGTH)->second.c_str());
	this->body_ = tempBuffer.substr(0, contentLength);

	if (this->body_.length() == contentLength)
		this->complete_ = true;
	if (this->body_.length() > contentLength)
		return false;

	return true;
}

bool Request::tryParseChunkedBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd)
{
	std::size_t lineLength = std::string::npos;
	std::string tempBuffer;

	do
	{
		if (!isLong(*bodyIt))
			return false;

		lineLength = atoi((*bodyIt).c_str());

		if (lineLength != (bodyIt + 1)->length())
			return false;

		std::cout << "Length: " << lineLength << " | Line: " << *(bodyIt + 1) << std::endl;

		tempBuffer += *(bodyIt + 1);
		bodyIt += 2;
	} while (bodyIt != bodyEnd && lineLength > 0);

	if (lineLength == 0)
	{
		if (bodyIt != bodyEnd && !bodyIt->empty())
			return false;
		this->complete_ = true;
	}

	this->body_ = tempBuffer.substr();
	return true;
}

bool Request::tryParseBody(std::vector<std::string> &body)
{
	std::vector<std::string>::iterator bodyIt = std::find_if(body.begin(), body.end(), isEmpty) + 1;
	std::vector<std::string>::iterator bodyEnd = body.end();

	if (this->headers_.find(CONTENT_LENGTH) != this->headers_.end())
		return tryParseFullBody(bodyIt, bodyEnd);

	return tryParseChunkedBody(bodyIt, bodyEnd);
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

bool Request::tryParseFromBuffer()
{
	std::vector<std::string> bufferLines = split(this->buffer_, HEADER_LINE_SEP);

	if (this->target_.empty() && !tryParseRequestLine(bufferLines.front()))
		return true;

	if (this->headers_.empty() && !tryParseHeaders(bufferLines))
		return true;
	
	if (this->method_ == GET || this->method_ == DELETE)
		this->complete_ = true;
	
	if (this->method_ == POST && !tryParseBody(bufferLines))
		return true;

	return this->complete_;
}

std::ostream &operator<<(std::ostream &stream, Request &request)
{
	stream << "+-------------------------------------+" << std::endl;
	stream << "|               REQUEST               |" << std::endl;
	stream << "+-------------------------------------+" << std::endl;
	stream << std::endl;

	stream << "  - Method: ";
	if (request.method_ == GET)
		stream << "GET" << std::endl;
	else if (request.method_ == POST)
		stream << "POST" << std::endl;
	else if (request.method_ == DELETE)
		stream << "DELETE" << std::endl;
	else
		stream << "NOT ALLOWED" << std::endl;

	stream << "  - Target: " << request.target_ << std::endl;

	stream << "  - HTTP Version: ";
	stream << request.httpVersion_;
	if (request.httpVersion_.empty())
		stream << "Invalid";
	stream << std::endl;

	stream << "  - Headers:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = request.headers_.begin(); it != request.headers_.end(); ++it)
	{
		stream << "    - " << it->first << ": " << it->second << std::endl;
	}

	stream << "  - Is complete: " << (request.complete_ ? "true" : "false") << std::endl;

	stream << "  - Body:" << std::endl;
	stream << request.body_ << std::endl;

	stream << "-----" << std::endl;

	stream << "  - Buffer:" << std::endl;
	stream << request.buffer_ << std::endl;

	stream << "+-------------------------------------+" << std::endl;
	stream << "|           REQUEST FINISHED          |" << std::endl;
	stream << "+-------------------------------------+";

	return stream;
}
