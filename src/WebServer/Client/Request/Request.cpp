#include "Request.hpp"
#include "utils/string/string.hpp"
#include "utils/numeric/numeric.hpp"
#include <iostream>
#include <algorithm>

const std::string Request::tchars = "!#$%&'*+-.^_`|~";

Request::Request()
{
	this->method_ = M_NOT_IMPLEMENTED;
	this->complete_ = false;
}

Request::~Request()
{
}

t_method Request::getHTTPMethod(const std::string &method)
{
	if (method == "GET")
		return M_GET;
	else if (method == "POST")
		return M_POST;
	else if (method == "DELETE")
		return M_DELETE;
	return M_NOT_IMPLEMENTED;
}

bool Request::isValidHeaderKey(const std::string &key)
{
	u_char c;

	for (int i = 0; i < key.length(); i++)
	{
		c = static_cast<u_char>(key[i]);

		if (!isalpha(c) && tchars.find(c) == std::string::npos)
			return false;
	}
    return true;
}

bool Request::isValidHeaderValue(const std::string &value)
{
	u_char c;

	for (int i = 0; i < value.length(); i++)
	{
		c = static_cast<u_char>(value[i]);

		if (!isprint(c) && c != '\t')
			return false;
	}
    return true;
}

bool Request::isValidHeader(const std::string &key, const std::string &value)
{
    return isValidHeaderKey(key) && isValidHeaderValue(value);
}

bool Request::shouldWaitForData(const std::string &str)
{
	if (str.empty())
		return false;

	int i = 0;
	while (i < str.length())
	{
		if (!isdigit(str[i]))
			break;
		i++;
	}

	return (str[i] == '\r' && i == str.length() - 1);
}

bool Request::checkValidHTTPProtocol()
{
	if (httpVersion_ == VALID_PROTOCOL)
		return true;
	else
	{
		if (httpVersion_.length() != 8 || httpVersion_.substr(0, 5) != "HTTP/")
			return false;
		if (isdigit(httpVersion_.at(5)) && httpVersion_.at(6) == '.' && isdigit(httpVersion_.at(7)))
			return true;
		return false;
	}
}

bool Request::tryParseRequestLine(const std::string &string)
{
	std::vector<std::string> splittedLine = split(string, " ");
	if (splittedLine.size() != 3)
		return false;

	this->method_ = getHTTPMethod(splittedLine[0]);
	if (splittedLine[0].empty())
		return false;

	this->target_ = splittedLine[1];
	if (this->target_.empty() || this->target_.at(0) != '/') // No need to start with /?
		return false;

	this->httpVersion_ = splittedLine[2];
	if (!checkValidHTTPProtocol())
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
		if (!isValidHeader(splittedHeader[0], splittedHeader[1]))
			return false;

		this->headers_[splittedHeader[0]] = splittedHeader[1];
	}

	std::map<std::string, std::string>::iterator contentLengthIt = this->headers_.find(CONTENT_LENGTH);
	std::map<std::string, std::string>::iterator transferEncodingIt = this->headers_.find(TRANSFER_ENCODING);

	if ((this->headers_.find("Host") == this->headers_.end())
		|| (this->method_ == M_POST && contentLengthIt == this->headers_.end() && transferEncodingIt == this->headers_.end())
		|| (contentLengthIt != this->headers_.end() && transferEncodingIt != this->headers_.end())
		|| (contentLengthIt != this->headers_.end() && !isLong(contentLengthIt->second) && atol(contentLengthIt->second.c_str()) >= 0)
		|| (transferEncodingIt != this->headers_.end() && transferEncodingIt->second != "chunked"))
		return false;

	return true;
}

bool Request::tryParseFullBody()
{
	size_t bodyStartingPoint = this->buffer_.find(HEADERS_DELIM) + 4;
	std::string tempBuffer = this->buffer_.substr(bodyStartingPoint);

	if (this->headers_.find(CONTENT_LENGTH) == this->headers_.end())
	{

		if (!tempBuffer.empty())
			return false;
		this->complete_ = true;

		return true;
	}

	long contentLength = atol(this->headers_.find(CONTENT_LENGTH)->second.c_str());

	if (tempBuffer.length() == contentLength)
		this->complete_ = true;
	if (tempBuffer.length() > contentLength)
		return false;

	this->body_ = tempBuffer.substr(0, contentLength);

	return true;
}

bool Request::tryParseChunkedBody(std::vector<std::string>::iterator &bodyIt, std::vector<std::string>::iterator &bodyEnd)
{
	std::size_t lineLength = std::string::npos;
	std::string tempBuffer;

	do
	{
		if (!isLong(*bodyIt))
			return shouldWaitForData(*bodyIt);

		lineLength = atoi((*bodyIt).c_str());
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

	if (this->headers_.find(TRANSFER_ENCODING) != this->headers_.end())
		return tryParseChunkedBody(bodyIt, bodyEnd);

	return tryParseFullBody();
}

std::string Request::getBuffer()
{
	return this->buffer_;
}

std::string Request::getBody()
{
	return this->body_;
}

void Request::clear()
{
	this->method_ = M_NOT_IMPLEMENTED;
	this->target_.clear();
	this->httpVersion_.clear();
	this->headers_.clear();
	this->body_.clear();
	this->buffer_.clear();
	this->complete_ = false;
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
		return false;

	if (this->headers_.empty() && !tryParseHeaders(bufferLines))
		return false;

	if (!tryParseBody(bufferLines))
		return false;

	return this->complete_;
}

t_method Request::getMethod()
{
    return this->method_;
}

std::string Request::getTarget()
{
    return this->target_;
}

std::string Request::getHTTPVersion()
{
    return this->httpVersion_;
}

std::map<std::string, std::string> Request::getHeaders()
{
    return this->headers_;
}

void Request::setComplete(bool status)
{
	this->complete_ = status;
}

void Request::setMethod(t_method method)
{
	this->method_ = method;
}

bool Request::isComplete()
{
    return this->complete_;
}

std::ostream &operator<<(std::ostream &stream, Request &request)
{
	stream << "+-------------------------------------+" << std::endl;
	stream << "|               REQUEST               |" << std::endl;
	stream << "+-------------------------------------+" << std::endl;
	stream << std::endl;

	stream << "  - Method: ";
	if (request.method_ == M_GET)
		stream << "GET" << std::endl;
	else if (request.method_ == M_POST)
		stream << "POST" << std::endl;
	else if (request.method_ == M_DELETE)
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

	stream << std::endl;
	stream << "+-------------------------------------+" << std::endl;
	stream << "|           REQUEST FINISHED          |" << std::endl;
	stream << "+-------------------------------------+";

	return stream;
}
