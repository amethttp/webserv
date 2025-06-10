#include "Response.hpp"
#include "Request/Request.hpp"
#include "Server/Server.hpp"
#include "Server/Location/Location.hpp"

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

bool Response::methodGet()
{

}

bool Response::methodPost()
{

}

bool Response::methodDelete()
{

}

bool Response::tryBuild(Request &request)
{
	Server testServer;
	Location testLocation;

	switch (request.getMethod())
	{
		case GET:
			return this->methodGet();
		case POST:
			return this->methodPost();
		case DELETE:
			return this->methodDelete();
		
		default:
			throw std::runtime_error("Not implemented (501)**");
	}
}
