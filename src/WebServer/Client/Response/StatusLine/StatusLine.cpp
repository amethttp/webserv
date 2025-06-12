#include "StatusLine.hpp"
#include <sstream>

StatusLine::StatusLine()
{
}

void StatusLine::setStatusLine(int code, const std::string &message)
{
	this->code_ = code;
	this->httpVersion_ = "HTTP/1.1";
	this->statusMessage_ = message;
}

std::string &operator+=(std::string &string, StatusLine &sl)
{
	std::ostringstream res;

	res << sl.httpVersion_ << " "
		<< sl.code_ << " "
		<< sl.statusMessage_
	<< "\r\n";

	string += res.str();

	return string;
}

std::ostream &operator<<(std::ostream &stream, StatusLine &sl)
{
	stream
		<< sl.httpVersion_ << " "
		<< sl.code_ << " "
		<< sl.statusMessage_
	<< "\r\n";
	
	return stream;
}

StatusLine::~StatusLine()
{
}
