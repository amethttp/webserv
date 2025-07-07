#include "StatusLine.hpp"
#include <sstream>

StatusLine::StatusLine()
{
}

void StatusLine::clear()
{
	this->code_ = (t_httpCode)0;
	this->httpVersion_.clear();
	this->statusMessage_.clear();
}

void StatusLine::setFields(t_httpCode code, const std::string &message)
{
	this->code_ = code;
	this->httpVersion_ = "HTTP/1.1";
	this->statusMessage_ = message;
}

t_httpCode StatusLine::getCode() const
{
    return this->code_;
}

std::string &operator+=(std::string &string, StatusLine &sl)
{
	std::ostringstream res;

	if (sl.code_ == 0)
		return string;

	res << sl.httpVersion_ << " "
		<< sl.code_ << " "
		<< sl.statusMessage_
	<< "\r\n";

	string += res.str();

	return string;
}

std::ostream &operator<<(std::ostream &stream, StatusLine &sl)
{
	if (sl.code_ == 0)
		return stream;

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
