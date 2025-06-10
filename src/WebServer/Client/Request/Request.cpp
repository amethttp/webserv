#include "Request.hpp"
#include "utils/string/string.hpp"
#include "utils/numeric/numeric.hpp"
#include <iostream>
#include <algorithm>

const std::string Request::tchars = "!#$%&'*+-.^_`|~";

Request::Request()
{
	this->method_ = NOT_IMPLEMENTED;
	this->valid_ = false;
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
	return NOT_IMPLEMENTED;
}

bool Request::isTokenChar(const char c)
{
	return isalnum(c) || tchars.find(c) != std::string::npos;
}

bool Request::isToken(const std::string &string)
{
	if (string.empty())
		return false;

	u_char c;
	for (int i = 0; i < string.length(); i++)
	{
		c = static_cast<u_char>(string[i]);

		if (!isTokenChar(string[i]))
			return false;
	}
	return true;
}

bool Request::isValidHeaderKey(const std::string &key)
{
    return isToken(key);
}

bool Request::isValidHeaderValue(const std::string &value)
{
	if (value.empty())
		return false;
	
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

bool Request::hasValidHost()
{
	std::map<std::string, std::string>::iterator hostIt = this->headers_.find("Host");

	if (hostIt == this->headers_.end())
		return false;
	
	if (hostIt->second.empty())
		return false;

	return true;
}

bool Request::isValidChunkExtensionValue(const std::string &chunkExtensionValue)
{
	if (chunkExtensionValue.empty())
		return false;

	size_t firstquotePos = chunkExtensionValue.find_first_of('"');
	size_t lastQuotePos = chunkExtensionValue.find_first_of('"', firstquotePos + 1);

	if (firstquotePos != std::string::npos)
	{
		if (firstquotePos != 0 || lastQuotePos != chunkExtensionValue.length() - 1)
			return false;
		return true;
	}

	return isToken(chunkExtensionValue);
}

bool Request::isValidChunkExtension(const std::string &chunkExtension)
{
	if (chunkExtension.empty())
		return false;

	std::vector<std::string> splittedExtension = split(chunkExtension, "=");

	if (splittedExtension.size() > 2)
		return false;
	if (!isToken(splittedExtension[0]))
		return false;
	if (splittedExtension.size() == 2
		&& !isValidChunkExtensionValue(splittedExtension[1]))
		return false;

	return true;
}

bool Request::isValidChunkSize(const std::string &string)
{
	if (string.empty())
		return false;

	std::vector<std::string> splittedChunk = split(string, ";");

	if (!isHex(splittedChunk[0]))
		return false;
	
	for (std::vector<std::string>::iterator it = splittedChunk.begin() + 1; it != splittedChunk.end(); ++it)
	{
		if (!isValidChunkExtension(*it) || it->length() > MAX_URI_LENGTH)
			return false;
	}
	return true;
}

size_t Request::getChunkSize(const std::string &string)
{
	size_t extPos = string.find(";");
	std::string chunkSizeStr = string.substr(0, extPos);

	return hexToDec(chunkSizeStr);
}

bool Request::shouldWaitForData(const std::string &str)
{
	if (str.empty())
		return false;

	std::vector<std::string> splittedChunk = split(str, ";");

	for (int i = 0; i < splittedChunk[0].length(); i++)
	{		
		if (!isHex(&splittedChunk[0][i]))
			return splittedChunk[0][i] == '\r' && i == splittedChunk[0].length() - 1;
	}
	
	for (std::vector<std::string>::iterator it = splittedChunk.begin() + 1; it != splittedChunk.end(); ++it)
	{
		if (it->length() > MAX_URI_LENGTH)
			return false;
		if (!isValidChunkExtension(*it))
		{
			if (it->empty() || it != splittedChunk.end() - 1)
				return false;

			std::vector<std::string> splittedExtension = split(*it, "=");

			if (splittedExtension.size() > 2)
				return false;
			if (!isToken(splittedExtension[0]))
				return false;
			if (splittedExtension[1].empty())
				return false;

			size_t firstquotePos = splittedExtension[1].find_first_of('"');

			if (firstquotePos != std::string::npos)
				return false;

			for (int i = 0; i < splittedExtension[1].length(); i++)
			{		
				if (!isTokenChar(splittedExtension[1][i]))
					return splittedExtension[1][i] == '\r' && i == splittedChunk[0].length() - 1;
			}
		}
	}
	return true;
}

bool Request::tryParseRequestLine(const std::string &string)
{
	std::vector<std::string> splittedLine = split(string, " ");
	if (splittedLine.size() != 3)
		return false;

	this->method_ = getHTTPMethod(splittedLine[0]);
	if (this->method_ == NOT_IMPLEMENTED)
		return false;

	this->target_ = splittedLine[1];
	if (this->target_.empty() || this->target_.length() > MAX_URI_LENGTH
		|| this->target_.at(0) != '/' || this->target_.find('\t') != std::string::npos)
		return false;

	this->httpVersion_ = splittedLine[2];
	if (this->httpVersion_ != VALID_PROTOCOL)
		return false;

	return true;
}

bool Request::tryParseHeaders(std::vector<std::string> &headers)
{
	for (std::vector<std::string>::iterator it = headers.begin() + 1; it != headers.end() && !it->empty(); ++it)
	{
		std::vector<std::string> splittedHeader = split(*it, ":");

		if (splittedHeader.size() != 2)
			return false;
		if (!isValidHeader(splittedHeader[0], splittedHeader[1]))
			return false;
		if (splittedHeader[0] == "Host" && this->headers_.find(splittedHeader[0]) != this->headers_.end())
			return false;

		this->headers_[splittedHeader[0]] = trim(splittedHeader[1], " \t");
	}

	std::map<std::string, std::string>::iterator contentLengthIt = this->headers_.find(CONTENT_LENGTH);
	std::map<std::string, std::string>::iterator transferEncodingIt = this->headers_.find(TRANSFER_ENCODING);

	if (!hasValidHost()
		|| (this->method_ == POST && contentLengthIt == this->headers_.end() && transferEncodingIt == this->headers_.end())
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
		this->valid_ = true;
		return true;
	}

	long contentLength = atol(this->headers_.find(CONTENT_LENGTH)->second.c_str());

	if (tempBuffer.length() == contentLength)
		this->valid_ = true;
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
		if (!isValidChunkSize(*bodyIt))
			return shouldWaitForData(*bodyIt);
		
		lineLength = getChunkSize(*bodyIt);
		tempBuffer += *(bodyIt + 1);
		bodyIt += 2;
	} while (bodyIt != bodyEnd && lineLength > 0);

	if (lineLength == 0)
	{
		if (bodyIt != bodyEnd && !bodyIt->empty())
			return false;
		this->valid_ = true;
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

void Request::clear()
{
	this->method_ = NOT_IMPLEMENTED;
	this->target_.clear();
	this->httpVersion_.clear();
	this->headers_.clear();
	this->body_.clear();
	this->buffer_.clear();
	this->valid_ = false;
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

	if (!tryParseBody(bufferLines))
		return true;

	return this->valid_;
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

	stream << "  - Is valid: " << (request.valid_ ? "true" : "false") << std::endl;

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
