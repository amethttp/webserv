#include "Connection.hpp"
#include <iostream>
#include "Request/RequestFactory/RequestFactory.hpp"

uint32_t Connection::idCounter_ = 1;

static std::map<t_httpCode, std::string> initializeErrorDict()
{
	std::map<t_httpCode, std::string> tempMap;

	tempMap[CONTINUE] = "Continue";
	tempMap[SWITCHING_PROTOCOLS] = "Switching Protocols";
	tempMap[OK] = "OK";
	tempMap[CREATED] = "Created";
	tempMap[ACCEPTED] = "Accepted";
	tempMap[NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
	tempMap[NO_CONTENT] = "No Content";
	tempMap[RESET_CONTENT] = "Reset Content";
	tempMap[PARTIAL_CONTENT] = "Partial Content";
	tempMap[MULTIPLE_CHOICES] = "Multiple Choices";
	tempMap[MOVED_PERMANENTLY] = "Moved Permanently";
	tempMap[FOUND] = "Found";
	tempMap[SEE_OTHER] = "See Other";
	tempMap[NOT_MODIFIED] = "Not Modified";
	tempMap[USE_PROXY] = "Use Proxy";
	tempMap[TEMPORARY_REDIRECT] = "Temporary Redirect";
	tempMap[BAD_REQUEST] = "Bad Request";
	tempMap[UNAUTHORIZED] = "Unauthorized";
	tempMap[PAYMENT_REQUIRED] = "Payment Required";
	tempMap[FORBIDDEN] = "Forbidden";
	tempMap[NOT_FOUND] = "Not Found";
	tempMap[METHOD_NOT_ALLOWED] = "Method Not Allowed";
	tempMap[NOT_ACCEPTABLE] = "Not Acceptable";
	tempMap[PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
	tempMap[REQUEST_TIME_OUT] = "Request Time-out";
	tempMap[CONFLICT] = "Conflict";
	tempMap[GONE] = "Gone";
	tempMap[LENGTH_REQUIRED] = "Length Required";
	tempMap[PRECONDITION_FAILED] = "Precondition Failed";
	tempMap[REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large";
	tempMap[REQUEST_URI_TOO_LARGE] = "Request-URI Too Large";
	tempMap[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	tempMap[REQUESTED_RANGE_NOT_SATISFIABLE] = "Requested range not satisfiable";
	tempMap[EXPECTATION_FAILED] = "Expectation Failed";
	tempMap[INTERNAL_SERVER_ERROR] = "Internal Server Error";
	tempMap[NOT_IMPLEMENTED] = "Not Implemented";
	tempMap[BAD_GATEWAY] = "Bad Gateway";
	tempMap[SERVICE_UNAVAILABLE] = "Service Unavailable";
	tempMap[GATEWAY_TIME_OUT] = "Gateway Time-out";
	tempMap[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version not supported";

	return tempMap;
}

std::map<t_httpCode, std::string> Connection::errorDict_ = initializeErrorDict();

static std::map<std::string, std::string> initializeExtensionDict()
{
	std::map<std::string, std::string> tempMap;

    tempMap[".txt"]  = "text/plain";
    tempMap[".html"] = "text/html";
	tempMap[".htm"] = "text/html";
    tempMap[".css"]  = "text/css";
    tempMap[".js"]   = "application/javascript";
    tempMap[".json"] = "application/json";
    tempMap[".xml"]  = "application/xml";
    tempMap[".png"]  = "image/png";
    tempMap[".jpg"]  = "image/jpeg";
    tempMap[".jpeg"] = "image/jpeg";
    tempMap[".gif"]  = "image/gif";
    tempMap[".ico"]  = "image/x-icon";
    tempMap[".pdf"]  = "application/pdf";
    tempMap[".zip"]  = "application/zip";
    tempMap[".tar"]  = "application/x-tar";

	return tempMap;
}

std::map<std::string, std::string> Connection::extensionTypesDict_ = initializeExtensionDict();

std::string Connection::getHttpErrorMsg(t_httpCode code)
{
	std::string res = "";

	if (errorDict_.find(code) != errorDict_.end())
		res = errorDict_[code];

	return res;
}

std::string Connection::getExtensionType(std::string extension)
{
	std::string res = "";

	if (extensionTypesDict_.find(extension) != extensionTypesDict_.end())
		res = extensionTypesDict_[extension];

	return res;
}

Connection::Connection()
{
	this->id_ = Connection::idCounter_;
	this->lastReceivedPacket_ = std::time(NULL);
	Connection::idCounter_++;
}

Connection::~Connection()
{
}

int Connection::getId()
{
	return this->id_;
}

fd_t Connection::getFd()
{
	return this->fd_;
}

time_t Connection::getLastReceivedPacket() const
{
    return this->lastReceivedPacket_;
}

t_httpCode Connection::getResponseStatus() const
{
    return this->response_.statusLine_.getCode();
}

std::string Connection::getResponseBuffer() const
{
	return this->response_.buffer_;
}

const t_Request &Connection::getRequest() const
{
	return this->request_;
}

const std::string &Connection::getRequestBuffer() const
{
	return this->request_.buffer;
}

void Connection::clearRequestBuffer()
{
	this->request_.buffer.clear();
}

void Connection::setFd(fd_t fd)
{
	this->fd_ = fd;
}

void Connection::updateLastReceivedPacket(char *buffer)
{
	this->appendToRequestBuffer(buffer);
	this->lastReceivedPacket_ = std::time(NULL);
}

void Connection::appendToRequestBuffer(const char *toAppend)
{
	this->request_.buffer.append(toAppend);
}

bool Connection::hasPendingRequest()
{
	return !this->request_.buffer.empty();
}

void Connection::eraseResponse(size_t bytesToErase)
{
	this->response_.buffer_.erase(0, bytesToErase);
}

bool Connection::shouldClose()
{
    if (this->response_.headers_.contains(CONNECTION))
	{
		if (this->response_.headers_.getHeaderValue(CONNECTION) == "close")
			return C_CLOSE;
	}

	return C_KEEP_ALIVE;
}

void Connection::setRequest(t_Request request)
{
	this->request_ = request;
}

void Connection::buildResponse(t_HandlingResult &result)
{
	this->response_ = ResponseFactory::create(result);
}

void Connection::buildResponse(t_httpCode code, t_connection mode)
{
	this->response_ = ResponseFactory::create(code, mode);
}
