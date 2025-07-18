#include "Client.hpp"

#include <iostream>

uint32_t Client::idCounter_ = 1;

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

std::map<t_httpCode, std::string> Client::errorDict_ = initializeErrorDict();

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

std::map<std::string, std::string> Client::extensionTypesDict_ = initializeExtensionDict();

std::string Client::getHttpErrorMsg(t_httpCode code)
{
	std::string res = "";

	if (errorDict_.find(code) != errorDict_.end())
		res = errorDict_[code];

	return res;
}

std::string Client::getExtensionType(std::string extension)
{
	std::string res = "";

	if (extensionTypesDict_.find(extension) != extensionTypesDict_.end())
		res = extensionTypesDict_[extension];

	return res;
}

Client::Client()
{
	this->id_ = Client::idCounter_;
	this->lastReceivedPacket_ = std::time(NULL);
	Client::idCounter_++;
}

Client::~Client()
{
}

int Client::getId()
{
	return this->id_;
}

fd_t Client::getFd()
{
	return this->fd_;
}

time_t Client::getLastReceivedPacket() const
{
    return this->lastReceivedPacket_;
}

std::string Client::getStringifiedResponse()
{
	std::ostringstream res;

	res << this->response_.statusLine_;
	res << this->response_.headers_;
	res << this->response_.body_.content;

	return res.str();
}

void Client::updateResponse()
{
	if (this->responseBuffer_.empty())
		this->responseBuffer_ = getStringifiedResponse();
}

std::string Client::getStringifiedRequest()
{
	return this->request_.getBuffer();
}

Request Client::getRequest() const
{
    return this->request_;
}

t_httpCode Client::getResponseStatus() const
{
    return this->response_.statusLine_.getCode();
}

std::string Client::getResponseBuffer() const
{
	return this->responseBuffer_;
}

void Client::setFd(fd_t fd)
{
	this->fd_ = fd;
}

void Client::updateLastReceivedPacket()
{
	this->lastReceivedPacket_ = std::time(NULL);
}

bool Client::hasFullRequestHeaders()
{
	return this->request_.hasFullHeaders();
}

void Client::buildRequest()
{
	this->request_.setComplete(request_.tryParseFromBuffer());

	// std::cout << this->request_ << std::endl;
	// std::cout << "Should build a response: " << (this->request_.isComplete() ? "true" : "false") << std::endl;
}

void Client::appendRequest(char *request)
{
	this->request_.appendBuffer(request);
}

bool Client::hasPendingRequest()
{
	return !this->request_.getBuffer().empty();
}

void Client::clearRequest()
{
	this->request_.clear();
}

void Client::eraseResponse(size_t bytesToErase)
{
	this->responseBuffer_.erase(0, bytesToErase);
}

bool Client::shouldClose()
{
    if (this->response_.headers_.contains("Connection"))
	{
		if (this->response_.headers_.getHeader("Connection").getValue() == "close")
			return C_CLOSE;
	}

	return C_KEEP_ALIVE;

}

void Client::buildResponse(std::vector<Server *> &servers)
{
	Server *server;
	Location *location;
	HandlingResult res;
	this->responseBuffer_.clear();

	server = ServerMatcher::matchServer(request_, servers);
	location = LocationMatcher::matchLocation(request_, *server);

	res = RequestHandler::handleRequest(request_, *location, *server);
	if (res.isCGI_)
		this->responseBuffer_ = res.tempBody_.content;
	else
		response_ = ResponseFactory::create(res);
}

void Client::buildResponse(t_httpCode code, t_connection mode)
{
	this->responseBuffer_.clear();
	this->response_ = ResponseFactory::create(code, mode);
}
