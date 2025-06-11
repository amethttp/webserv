#include <iostream>
#include "Response.hpp"

#include <fstream>
#include <sstream>

static std::map<int, std::string> initializeDict()
{
	std::map<int, std::string> tempMap;

	tempMap.insert(std::pair<int, std::string>(100, "Continue"));
	tempMap.insert(std::pair<int, std::string>(101, "Switching Protocols"));
	tempMap.insert(std::pair<int, std::string>(200, "OK"));
	tempMap.insert(std::pair<int, std::string>(201, "Created"));
	tempMap.insert(std::pair<int, std::string>(202, "Accepted"));
	tempMap.insert(std::pair<int, std::string>(203, "Non-Authoritative Information"));
	tempMap.insert(std::pair<int, std::string>(204, "No Content"));
	tempMap.insert(std::pair<int, std::string>(205, "Reset Content"));
	tempMap.insert(std::pair<int, std::string>(206, "Partial Content"));
	tempMap.insert(std::pair<int, std::string>(300, "Multiple Choices"));
	tempMap.insert(std::pair<int, std::string>(301, "Moved Permanently"));
	tempMap.insert(std::pair<int, std::string>(302, "Found"));
	tempMap.insert(std::pair<int, std::string>(303, "See Other"));
	tempMap.insert(std::pair<int, std::string>(304, "Not Modified"));
	tempMap.insert(std::pair<int, std::string>(305, "Use Proxy"));
	tempMap.insert(std::pair<int, std::string>(307, "Temporary Redirect"));
	tempMap.insert(std::pair<int, std::string>(400, "Bad Request"));
	tempMap.insert(std::pair<int, std::string>(401, "Unauthorized"));
	tempMap.insert(std::pair<int, std::string>(402, "Payment Required"));
	tempMap.insert(std::pair<int, std::string>(403, "Forbidden"));
	tempMap.insert(std::pair<int, std::string>(404, "Not Found"));
	tempMap.insert(std::pair<int, std::string>(405, "Method Not Allowed"));
	tempMap.insert(std::pair<int, std::string>(406, "Not Acceptable"));
	tempMap.insert(std::pair<int, std::string>(407, "Proxy Authentication Required"));
	tempMap.insert(std::pair<int, std::string>(408, "Request Time-out"));
	tempMap.insert(std::pair<int, std::string>(409, "Conflict"));
	tempMap.insert(std::pair<int, std::string>(410, "Gone"));
	tempMap.insert(std::pair<int, std::string>(411, "Length Required"));
	tempMap.insert(std::pair<int, std::string>(412, "Precondition Failed"));
	tempMap.insert(std::pair<int, std::string>(413, "Request Entity Too Large"));
	tempMap.insert(std::pair<int, std::string>(414, "Request-URI Too Large"));
	tempMap.insert(std::pair<int, std::string>(415, "Unsupported Media Type"));
	tempMap.insert(std::pair<int, std::string>(416, "Requested range not satisfiable"));
	tempMap.insert(std::pair<int, std::string>(417, "Expectation Failed"));
	tempMap.insert(std::pair<int, std::string>(500, "Internal Server Error"));
	tempMap.insert(std::pair<int, std::string>(501, "Not Implemented"));
	tempMap.insert(std::pair<int, std::string>(502, "Bad Gateway"));
	tempMap.insert(std::pair<int, std::string>(503, "Service Unavailable"));
	tempMap.insert(std::pair<int, std::string>(504, "Gateway Time-out"));
	tempMap.insert(std::pair<int, std::string>(505, "HTTP Version not supported"));

	return tempMap;
}

std::map<int, std::string> Response::errorDict_ = initializeDict();

std::string Response::toString()
{
	std::string res = "";

	res += this->statusLine;
	for (std::map<std::string, std::string>::iterator it = this->headers_.begin(); it != this->headers_.end(); ++it) {
		res += (*it).first + " " + (*it).second + "\r\n";
	}
	res += "\r\n";
	res += body_;

	return res;
}

void Response::setBuffer(const std::string &buffer)
{
	this->buffer_ = buffer;
}

void Response::setBody(const std::string &body)
{
	this->body_ = body;
}

std::string Response::getBuffer()
{
	return this->buffer_;
}

std::string Response::getBody()
{
	return this->body_;
}

void Response::eraseBuffer(size_t bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}

bool Response::methodGet(Request &request, Server &server, Location &location)
{
	std::fstream file;
	std::ostringstream length;
	std::string path;

	path = location.getPath() + request.target_;
    file.open(path.c_str());
    if (file.is_open())
    {
        while (getline(file, this->body_, '\0')) { }
    }
    else
        throw (std::runtime_error("Error: Couldn't open file"));
    file.close();
	this->headers_.insert(std::pair<std::string, std::string>("content-type", "text/html"));
	length << this->body_.size();
	this->headers_.insert(std::pair<std::string, std::string>("content-length", length.str()));
	this->statusLine.setStatusLine(200, "HTTP/1.1", "OK");

	return true;
}

bool Response::methodPost(Request &request, Server &server, Location &location)
{
	std::cout << "POSTTT" << std::endl;
	return true;
}

bool Response::methodDelete(Request &request, Server &server, Location &location)
{
	std::cout << "DELETE" << std::endl;
	return true;
}

// first check that the method is valid
// then check for keep alive / other headers of interest 
// finally execute method and set headers accordingly
bool Response::tryBuild(Request &request)
{
	Server testServer;
	Location testLocation;
	std::set<method_t> allowedMethods;

	allowedMethods.insert(GET);
	allowedMethods.insert(POST);
	testLocation.setPath("tests");
	testLocation.setRoot("/");
	testLocation.setMethods(allowedMethods);

	switch (request.getMethod())
	{
		case GET:
			this->methodGet(request, testServer, testLocation);
			break ;
		case POST:
			this->methodPost(request, testServer, testLocation);
			break ;
		case DELETE:
			this->methodDelete(request, testServer, testLocation);
			break ;
		
		default:
			throw std::runtime_error("Not implemented (501)**");
	}
	this->buffer_ = this->toString();

	return true;
}
