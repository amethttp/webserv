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

// Decide if we normalize slashes or not || Nginx default beaviour doesnt do it
static void removeTrailingSlashes(std::string &str)
{
	if (*(str.rbegin()) == '/' && str.length() > 1)
		str.erase(str.length() - 1);
}

static int countMatchingCharacters(std::string &base, std::string target)
{
	int res = 0;
	int tmp = 0;
	int i = 0;

	// removeTrailingSlashes(target);
	while (target[i] && base[i])
	{
		if (target[i] != base[i])
			break ;
		tmp++;
		if (target[i] == '/' && base[i] == '/')
		{
			res += tmp;
			tmp = 0;
		}
		i++;
	}
	if (target[i] || base[i])
		return 0;
	return res;
}

// Decide on no locations defined on server
static Location matchLocation(Request &request, Server &server)
{
	int matchIndex = 0;
	int matchLength = 0;
	int longestMatch = 0;
	std::string targetRoute = request.getTarget();
	std::vector<Location> locations = server.getLocations();

	for (size_t i = 0; i < locations.size(); ++i)
	{
		matchLength = countMatchingCharacters(targetRoute, locations[i].getPath());
		if (matchLength > longestMatch)
		{
			matchIndex = i;
			longestMatch = matchLength;
		}
	}

	return locations[matchIndex];
}

static Location getLocationPH(Request &request, Server &server)
{
	Location location;
	std::vector<Location> testLocations;
	std::vector<std::string> indexes;
	std::set<t_method> allowedMethods;
	t_return ret;

	indexes.push_back("index");
	indexes.push_back("index.html");
	indexes.push_back("test.html");
	indexes.push_back("test2.html");
	allowedMethods.insert(M_GET);
	allowedMethods.insert(M_POST);
	location.setRoot("tests/www");
	location.setPath("/");
	location.setAutoIndex(true);
	location.setIndexList(indexes);
	location.setMethods(allowedMethods);
	testLocations.push_back(location);

	location.setRoot("");
	location.setPath("/ok/testindex/index");
	ret.code = MOVED_PERMANENTLY;
	ret.path = "/ok/testindex/index/";
	location.setReturn(ret);
	location.setAutoIndex(true);
	location.setIndexList(indexes);
	location.setMethods(allowedMethods);
	testLocations.push_back(location);
	server.setLocations(testLocations);
	server.setUploadPath("tests/www/uploads/");

	return matchLocation(request, server); // this prob belongs to server
}

// resolve no name match // same server name dieferent ports... || need to know the port etc..
// Handle when server / location fields might not be filled... eg: no names...
static Server getServerPH(Request &request, std::vector<Server> &serverList)
{
	std::string host = request.getHeaders()["Host"];

	for (std::vector<Server>::iterator serverIt = serverList.begin(); serverIt != serverList.end(); ++serverIt)
	{
		for (std::vector<std::string>::iterator name = serverIt->getNames().begin(); name != serverIt->getNames().end(); ++name)
		{
			if (*name == host)
				return *serverIt;
		}
	}

	return *serverList.begin();
}

void Client::executeRequest(std::vector<Server> &serversList)
{
	Server server = getServerPH(this->request_, serversList);
	Location location = getLocationPH(this->request_, server);
	Parameters responseParams(this->request_, location);
	responseParams.uploadPath_ = "tests/www/"; // from server??

	if (this->request_.getHTTPVersion() == "HTTP/1.1")
		this->response_.build(responseParams);
	else
		this->response_.build(HTTP_VERSION_NOT_SUPPORTED, C_KEEP_ALIVE);
}

// instead of build response implement request executor ?? 
void Client::buildResponse(std::vector<Server> &servers)
{
	if (this->request_.isComplete())
		this->executeRequest(servers);
	else
		this->response_.build(BAD_REQUEST, C_CLOSE);
}

void Client::buildResponse(t_httpCode code, t_connection mode)
{
	this->response_.build(code, mode);
}
