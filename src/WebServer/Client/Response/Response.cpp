#include <iostream>
#include "Response.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <ctime>

static std::map<int, std::string> initializeErrorDict()
{
	std::map<int, std::string> tempMap;

	tempMap[100] = "Continue";
	tempMap[101] = "Switching Protocols";
	tempMap[200] = "OK";
	tempMap[201] = "Created";
	tempMap[202] = "Accepted";
	tempMap[203] = "Non-Authoritative Information";
	tempMap[204] = "No Content";
	tempMap[205] = "Reset Content";
	tempMap[206] = "Partial Content";
	tempMap[300] = "Multiple Choices";
	tempMap[301] = "Moved Permanently";
	tempMap[302] = "Found";
	tempMap[303] = "See Other";
	tempMap[304] = "Not Modified";
	tempMap[305] = "Use Proxy";
	tempMap[307] = "Temporary Redirect";
	tempMap[400] = "Bad Request";
	tempMap[401] = "Unauthorized";
	tempMap[402] = "Payment Required";
	tempMap[403] = "Forbidden";
	tempMap[404] = "Not Found";
	tempMap[405] = "Method Not Allowed";
	tempMap[406] = "Not Acceptable";
	tempMap[407] = "Proxy Authentication Required";
	tempMap[408] = "Request Time-out";
	tempMap[409] = "Conflict";
	tempMap[410] = "Gone";
	tempMap[411] = "Length Required";
	tempMap[412] = "Precondition Failed";
	tempMap[413] = "Request Entity Too Large";
	tempMap[414] = "Request-URI Too Large";
	tempMap[415] = "Unsupported Media Type";
	tempMap[416] = "Requested range not satisfiable";
	tempMap[417] = "Expectation Failed";
	tempMap[500] = "Internal Server Error";
	tempMap[501] = "Not Implemented";
	tempMap[502] = "Bad Gateway";
	tempMap[503] = "Service Unavailable";
	tempMap[504] = "Gateway Time-out";
	tempMap[505] = "HTTP Version not supported";

	return tempMap;
}

std::map<int, std::string> Response::errorDict_ = initializeErrorDict();

static std::map<std::string, std::string> initializeExtensionDict()
{
	std::map<std::string, std::string> tempMap;

    tempMap[".txt"]  = "text/plain";
    tempMap[".html"] = "text/html";
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

std::string Response::toString()
{
	std::ostringstream res;

	res << this->statusLine;
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

void Response::eraseBuffer(size_t bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}

static void readFileToString(const char *filePath, std::string &str)
{
	struct stat st;
	std::ifstream file;

	file.open(filePath, std::ifstream::binary);
    if (!file.is_open())
	{
		file.close();
        throw (std::runtime_error("404? Couldn't open file"));
	}
	if (stat(filePath, &st))
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

void Response::setResponseHeaders()
{
	this->headers_.insert(std::pair<std::string, std::string>("Server", "Amethttp"));
	this->headers_.insert(std::pair<std::string, std::string>("Date", getImfFixdate()));
}

std::string Response::getMIME(std::string &target)
{
	int pos;
	std::string res = "text/plain";

	pos = target.rfind('.');
	if (pos != std::string::npos)
	{
		try
		{
			res = this->extensionTypesDict_.at(target.substr(pos));
		}
		catch(const std::out_of_range& e) {}
	}
	
	return res;
}

void Response::setRepresentationHeaders(std::string &target)
{
	std::ostringstream length;

	this->headers_.insert(std::pair<std::string, std::string>("Content-Type", this->getMIME(target)));
	length << this->body_.length();
	this->headers_.insert(std::pair<std::string, std::string>("Content-Length", length.str()));
}

// match the location outside this func and let it only receive the path to file
// status line OK outside this func when checked the return?
bool Response::methodGet(std::string &path)
{
	readFileToString(path.c_str(), this->body_);
	this->setResponseHeaders();
	this->setRepresentationHeaders(path);
	this->statusLine.setStatusLine(200, this->errorDict_.at(200));

	return true;
}

bool Response::methodPost(std::string &path)
{
	std::cout << "POSTTT" << std::endl;
	return true;
}

bool Response::methodDelete(std::string &path)
{
	if (std::remove(path.c_str()) < 0)
		throw std::runtime_error("404"); // 204 for consistency/idempotency, widely used...
	this->setResponseHeaders();
	this->statusLine.setStatusLine(204, this->errorDict_.at(204));

	return true;
}

// (We will get the server--> match the location)
// check that the method is valid (determined by location as well)
// then check for keep alive / other headers of interest 
// finally execute method and set headers accordingly
bool Response::tryBuild(Request &request)
{
	std::string path;
	Server testServer;
	Location testLocation;
	std::set<method_t> allowedMethods;

	allowedMethods.insert(GET);
	allowedMethods.insert(POST);
	testLocation.setPath("tests");
	testLocation.setRoot("/");
	testLocation.setMethods(allowedMethods);

	path = testLocation.getPath() + request.target_;

	switch (request.getMethod())
	{
		case GET:
			this->methodGet(path);
			break ;
		case POST:
			this->methodPost(path);
			break ;
		case DELETE:
			this->methodDelete(path);
			break ;
		
		default:
			throw std::runtime_error("Not implemented (501)**");
	}
	this->buffer_ = this->toString();

	return true;
}
