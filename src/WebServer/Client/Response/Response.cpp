#include <iostream>
#include "Response.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <unistd.h>

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

std::map<t_httpCode, std::string> Response::errorDict_ = initializeErrorDict();

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
}


std::string Response::toString()
{
	std::ostringstream res;

	res << this->statusLine_;
	for (std::map<std::string, std::string>::iterator it = this->headers_.begin(); it != this->headers_.end(); ++it) {
		res << (*it).first << ": " << (*it).second << "\r\n";
	}
	res << "\r\n";
	res << this->body_.content;

	return res.str();
}

void Response::setBuffer(const std::string &buffer)
{
	this->buffer_ = buffer;
}

std::string Response::getBuffer() const
{
	return this->buffer_;
}

t_httpCode Response::getStatusCode() const
{
	return this->statusLine_.getCode();
}

bool Response::getConnection() const
{
    if (this->headers_.find("Connection") != this->headers_.end())
	{
		if (this->headers_.at("Connection") == "close")
			return C_CLOSE;
	}

	return C_KEEP_ALIVE;
}

void Response::eraseBuffer(size_t bytesToErase)
{
	this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + bytesToErase);
}

static std::string getImfFixdate()
{
    std::time_t timeNow = std::time(NULL);
    std::tm *gmt = std::gmtime(&timeNow);
    std::string res;
	size_t resSize;
	
	res.resize(50);
    if ((resSize = std::strftime(&res[0], res.size(), "%a, %d %b %Y %H:%M:%S GMT", gmt)))
	{
		res.resize(resSize);
        return res;
	}
	return std::string("");
}

void Response::setStatusLine(t_httpCode code)
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

static bool isRedirection(t_httpCode code)
{
	return ((code / 100) * 100 == 300);
}

std::string Response::getRedirectionHTML(t_httpCode code, std::string &uri)
{
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		<<"<html>\n" 
		<< "<head><title>" << code << " " << this->errorDict_[code] << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << this->errorDict_[code] << "</h1>\n"
		<< "<p>The document has moved <a href=\"" << uri << "\">" << "here" << "</a>.</p>\n"
		<< "</body>\n"
		<< "</html>\n";
	
	return html.str();
}

void Response::setRedirectionResponse(t_httpCode code, std::string uri)
{
	this->headers_["Location"] = uri;
	this->body_.content = getRedirectionHTML(code, uri);
	this->body_.type = this->extensionTypesDict_[".html"];
}

void Response::setResponseHeaders(t_connection mode)
{
	this->headers_["Server"] = "Amethttp";
	this->headers_["Date"] = getImfFixdate();
	this->headers_["Connection"] = mode ? "close" : "keep-alive";
}


void Response::setRepresentationHeaders()
{
	std::ostringstream length;

	length << this->body_.content.length();
	this->headers_["Content-Length"] = length.str();
	if (!this->body_.content.empty())
		this->headers_["Content-Type"] = this->body_.type;
}

void Response::generateResponse(t_httpCode code, t_connection mode)
{
	setStatusLine(code);
	this->setResponseHeaders(mode);
	this->setRepresentationHeaders();
}

// Set necessary ??
static bool matchCustomErrorPage(t_httpCode code, Location &location, t_error_page &page)
{
	std::set<t_error_page> errorPages = location.getErrorPages();
	bzero(&page, sizeof(page));

	for (std::set<t_error_page>::iterator ite = errorPages.begin(); ite != errorPages.end(); ++ite)
	{
		if (ite->code == code)
		{
			page = *ite;
			return true;
		}
	}

	return false;
}

void Response::handleReturnDirective(t_return ret, t_connection mode)
{
	if (!ret.path.empty())
	{
		if (isRedirection(ret.code))
			this->setRedirectionResponse(ret.code, ret.path);
		else
			this->setBodyFromString(ret.path);
	}

	this->generateResponse(ret.code, mode);
}

void Response::build(Context &ctx) 
{
	this->clear();

	if (checkReturn(ctx.location_))
		this->handleReturnDirective(ctx.location_.getReturn(), ctx.getConnectionMode());
	else
		this->executeRequest(ctx);
	
	if (matchCustomErrorPage(code, ctx.location_, errorPage))
	{
		// config parse that an error page MUST have a URI page 
		this->setBodyFromFile(errorPage.page);
		this->generateResponse(errorPage.code, mode);
	}
	else
		this->generateResponse(code, mode);
}

void Response::build(t_httpCode code, t_connection mode)
{
	this->clear();

	this->generateResponse(code, mode);
}

void Response::clear()
{
	this->buffer_.clear();
	this->headers_.clear();
	this->body_.content.clear();
	this->body_.type.clear();
	this->statusLine_.clear();
}

Response::~Response()
{
}