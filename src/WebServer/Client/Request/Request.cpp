#include "Request.hpp"

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
