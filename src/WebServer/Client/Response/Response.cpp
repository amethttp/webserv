#include "Response.hpp"

std::string Response::toString()
{
	return this->buffer_;
}

void Response::setBuffer(const std::string &buffer)
{
	this->buffer_ = buffer;
}

void Response::eraseBuffer(int bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}
