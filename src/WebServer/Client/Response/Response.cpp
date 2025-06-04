#include "Response.hpp"

std::string Response::toString()
{
	return this->buffer_;
}

void Response::eraseBuffer(int bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}
