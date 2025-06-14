#include <iostream>
#include "Response.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <ctime>
#include <vector>

static std::map<httpError_t, std::string> initializeErrorDict()
{
	std::map<httpError_t, std::string> tempMap;

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

std::map<httpError_t, std::string> Response::errorDict_ = initializeErrorDict();

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

std::map<std::string, std::string> Response::extensionTypesDict_ = initializeExtensionDict();

Response::Response()
{
	this->endConnection_ = C_KEEP_ALIVE;
}

static int checkPath(std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return errno;

	return (st.st_mode & S_IFMT);
}

std::string Response::toString()
{
	std::ostringstream res;

	res << this->statusLine_;
	for (std::map<std::string, std::string>::iterator it = this->headers_.begin(); it != this->headers_.end(); ++it) {
		res << (*it).first << ": " << (*it).second << "\r\n";
	}
	res << "\r\n";
	res << body_;

	return res.str();
}

void Response::setBuffer(const std::string &buffer)
{
	this->buffer_ = buffer;
}

std::string Response::getBuffer()
{
	return this->buffer_;
}

std::string Response::getBody()
{
	return this->body_;
}

bool Response::getConnection()
{
    return this->endConnection_;
}

void Response::eraseBuffer(size_t bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}

static void readFileToString(const char *path, std::string &str)
{
	struct stat st;
	std::ifstream file(path, std::ifstream::binary);

    if (!file.is_open())
	{
		file.close();
        throw (std::runtime_error("Couldn't open file"));
	}
	if (stat(path, &st))
	{
		file.close();
		throw (std::runtime_error("Stat operation failed"));
	}
	str.resize(static_cast<size_t>(st.st_size));
	file.read(&str[0], static_cast<size_t>(st.st_size));
	if (!file)
	{
		file.close();
		throw (std::runtime_error("Reading operation failed"));
	}
    file.close();
}

static std::string getImfFixdate()
{
    std::time_t timeNow = std::time(NULL);
    std::tm *gmt = std::gmtime(&timeNow);
    std::string res;
	
	res.resize(30);
    if (std::strftime(&res[0], res.size(), "%a, %d %b %Y %H:%M:%S GMT", gmt))
        return res;
	return std::string("");
}

void Response::setStatusLine(httpError_t code)
{
	if (this->errorDict_.find(code) == this->errorDict_.end())
	{
		int defaultCode = (code / 100) * 100;
		if (defaultCode != 100 ||
			defaultCode != 200 ||
			defaultCode != 300 ||
			defaultCode != 400 ||
			defaultCode != 500)
		{
			throw std::runtime_error("Couldn't determine error code");			
		}
	}
	this->statusLine_.setFields(code, this->errorDict_[code]);
}

void Response::setResponseHeaders()
{
	this->headers_["Server"] = "Amethttp";
	this->headers_["Date"] = getImfFixdate();
	this->headers_["Connection"] = this->endConnection_ ? "close" : "keep-alive";
}

std::string Response::getMIME(std::string &target)
{
	int pos;
	std::string res = "text/plain";

	pos = target.rfind('.');
	if (pos != std::string::npos)
	{
		if (this->extensionTypesDict_.find(target.substr(pos)) != this->extensionTypesDict_.end())
			res = this->extensionTypesDict_[target.substr(pos)];
	}
	
	return res;
}

void Response::setRepresentationHeaders(std::string &target)
{
	std::ostringstream length;

	length << this->body_.length();
	this->headers_["Content-Type"] = this->getMIME(target);
	this->headers_["Content-Length"] = length.str();
}

void Response::generateResponse(httpError_t code)
{
	this->setResponseHeaders();
	setStatusLine(code);
}

void Response::methodGet()
{
	int statCheck;

	statCheck = checkPath(this->targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			readFileToString(this->targetPath_.c_str(), this->body_);
			this->setRepresentationHeaders(this->targetPath_);
			this->generateResponse(OK);
			break;
		case S_IFDIR:
			// autoindex html display...
			this->generateResponse(OK);
			break;
		case EACCES:
			this->generateResponse(FORBIDDEN);
			break;

		default:
			this->generateResponse(NOT_FOUND);
			break;
	}
}

void Response::methodPost()
{
	int statCheck;

	statCheck = checkPath(this->targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			// POSTTTTTT
			this->generateResponse(OK);
			break;
		case EACCES:
		case S_IFDIR:
			this->generateResponse(FORBIDDEN);
			break;

		default:
			this->generateResponse(NOT_FOUND);
			break;
	}

}

static void removeFile(const char *path)
{
	if (std::remove(path) < 0)
		throw std::runtime_error("Couldn't remove resource");
}

void Response::methodDelete()
{
	int statCheck;

	statCheck = checkPath(this->targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			removeFile(this->targetPath_.c_str());
			this->generateResponse(NO_CONTENT);
			break;
		case EACCES:
		case S_IFDIR:
			this->generateResponse(FORBIDDEN);
			break;

		default:
			this->generateResponse(NOT_FOUND);
			break;
	}
}

static Location matchLocation(Server &server, Request &request)
{
	return server.getLocations().at(0);
}

static std::string routeTarget(Location &location, Request &request)
{
	std::string path;

	path = location.getPath() + request.getTarget();

	return path;
}

static method_t fitMethod(method_t method, Location &location)
{
	std::set<method_t> allowedMethods = location.getMethods();

	if (allowedMethods.find(method) != allowedMethods.end())
	{
		return method;
	}

	return NOT_ALLOWED;
}

void Response::checkRequestHeaders(Request &request)
{
	std::map<std::string, std::string> reqHeaders = request.getHeaders();

	if (reqHeaders.find("Connection") != reqHeaders.end())
	{
		if (reqHeaders["Connection"] == "close")
			this->endConnection_ = C_CLOSE;
	}
}

void Response::executeRequest()
{
	switch (this->method_)
	{
		case GET:
			this->methodGet();
			break ;
		case POST:
			this->methodPost();
			break ;
		case DELETE:
			this->methodDelete();
			break ;
		
		default:
			this->generateResponse(NOT_IMPLEMENTED);
			break ;
	}
}

void Response::setParameters(Request &request)
{
	Server testServer;

	Location location;
	std::vector<Location> testLocations;
	std::set<method_t> allowedMethods;

	allowedMethods.insert(GET);
	allowedMethods.insert(POST);
	allowedMethods.insert(DELETE);
	location.setPath("tests");
	location.setRoot("/");
	location.setMethods(allowedMethods);
	testLocations.push_back(location);
	testServer.setLocations(testLocations);

	location = matchLocation(testServer, request);

	this->targetPath_ = routeTarget(location, request);
	this->method_ = fitMethod(request.getMethod(), location);

	this->checkRequestHeaders(request);
}

void Response::build(Request &request)
{
	if (!request.isComplete())
	{
		this->endConnection_ = C_CLOSE;
		this->generateResponse(BAD_REQUEST);
	}
	else
	{
		this->setParameters(request);
		this->executeRequest();
	}

	this->buffer_ = this->toString();
}

Response::~Response()
{
}