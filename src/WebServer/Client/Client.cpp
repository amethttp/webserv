#include "Client.hpp"

#include <iostream>

uint32_t Client::idCounter_ = 1;

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

time_t Client::getLastReceivedPacket()
{
    return this->lastReceivedPacket_;
}

std::string Client::getStringifiedResponse()
{
	if (this->response_.getBuffer().empty())
		this->response_.setBuffer(this->response_.toString());

	return this->response_.getBuffer();
}

std::string Client::getStringifiedRequest()
{
	return this->request_.getBuffer();
}

Request Client::getRequest()
{
    return this->request_;
}

t_httpCode Client::getResponseStatus()
{
    return this->response_.getStatusCode();
}

void Client::setFd(fd_t fd)
{
	this->fd_ = fd;
}

void Client::setResponseBuffer(const std::string &stringResponse)
{
	this->response_.setBuffer(stringResponse);
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
	this->response_.eraseBuffer(bytesToErase);
}

bool Client::shouldClose()
{
    return this->response_.getConnection();
}

static void setLocationsPH(Request &request, Server &server)
{
	Location location;
	std::vector<Location> testLocations;
	std::vector<std::string> indexes;
	std::set<t_method> allowedMethods;
	t_return ret;

	std::map<std::string, std::string> test;

	indexes.push_back("index");
	indexes.push_back("index.html");
	indexes.push_back("test.html");
	indexes.push_back("test2.html");
	allowedMethods.insert(M_GET);
	allowedMethods.insert(M_POST);
	test[".py"] = "/usr/bin/python3";
	test[".sh"] = "/bin/bash";

	location.setCGIs(test);
	location.setRoot("tests/www");
	location.setPath("/");
	location.setAutoIndex(true);
	location.setIndexList(indexes);
	location.setMethods(allowedMethods);
	testLocations.push_back(location);

	location.setRoot("tests/www");
	location.setPath("/test/index");
	ret.code = MOVED_PERMANENTLY;
	ret.path = "/test/index/";
	location.setReturn(ret);
	location.setAutoIndex(false);
	location.setIndexList(indexes);
	location.setMethods(allowedMethods);
	testLocations.push_back(location);
	server.setLocations(testLocations);
	server.setUploadPath("tests/www/uploads/");
}

void Client::buildResponse(std::vector<Server> &servers)
{
	Server server;
	Location location;
	ExecutionResult exeResult;
	
	

	server = ServerMatcher::matchServer(request_, servers);
	setLocationsPH(request_, server);
	location = LocationMatcher::matchLocation(request_, server);

	Context exeContext(request_, location, server);
	
	exeResult = RequestExecutor::executeRequest(exeContext);
	response_ = ResponseFactory::create(exeResult);
}

void Client::buildResponse(t_httpCode code, t_connection mode)
{
	this->response_.build(code, mode);
}
