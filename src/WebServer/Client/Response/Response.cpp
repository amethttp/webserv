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

static int checkPath(std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return errno;
	if (access(path.c_str(), R_OK))
		return EACCES;

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

static std::string readFileToString(const char *path)
{
	struct stat st;
	std::string str;
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

	return str;
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

void Response::setResponseHeaders(t_connection mode)
{
	this->headers_["Server"] = "Amethttp";
	this->headers_["Date"] = getImfFixdate();
	this->headers_["Connection"] = mode ? "close" : "keep-alive";
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

void Response::setRepresentationHeaders()
{
	std::ostringstream length;

	length << this->body_.content.length();
	this->headers_["Content-Length"] = length.str();
	if (!this->body_.content.empty())
		this->headers_["Content-Type"] = this->body_.type;
}

void Response::parseCustomPage(std::string &pagePath)
{
	this->body_.content = readFileToString(pagePath.c_str());
	this->body_.type = getMIME(pagePath);
}

void Response::generateResponse(error_page_t &customPage, t_connection mode)
{
	setStatusLine((t_httpCode)customPage.code);
	this->setResponseHeaders(mode);
	this->parseCustomPage(customPage.page);
	this->setRepresentationHeaders();
}

void Response::generateResponse(t_httpCode code, t_connection mode)
{
	setStatusLine(code);
	this->setResponseHeaders(mode);
	this->setRepresentationHeaders();
}

t_httpCode Response::getFile(std::string &target)
{
	this->body_.content = readFileToString(target.c_str());
	this->body_.type = getMIME(target);

	return OK;
}

static void startHTML(std::ostringstream &html, const std::string &targetName)
{
	html << "<!DOCTYPE html>\n"
		<< "<html>\n"
			<< "<head>\n"
				<< "<title>" << "Index of: " << targetName << "</title>\n"
				<< readFileToString(INDEX_STYLE)
			<< "</head>\n"
			<< "<body>\n"
				<< "<h2>" << "Index of: " << targetName << "</h2>\n"
					<< readFileToString(INDEX_FILE_LIST)
					<< "<ul>\n";
}

static void closeHTML(std::ostringstream &html)
{
	html << readFileToString(INDEX_CLOSE);
}

static void appendElementToHTML(std::ostringstream &html, std::string &targetPath, std::string &anchorName, std::string displayName, unsigned char type)
{
	struct stat st;
	std::ostringstream size;
	std::string lastModified = "";
	std::string filePath = targetPath + anchorName;
	if (!stat(filePath.c_str(), &st))
	{
		if (type != DT_DIR)
			size << (st.st_size / 1000) << " KB";
		lastModified = ctime(&st.st_mtime);
	}
	html << "<li class=\"file-item\">"
			<< "<div><a href=\"" << anchorName << "\">" << displayName <<"</a></div>\n"
			<< "<div>" << size.str() << "</div>\n"
			<< "<div>" << lastModified << "</div>\n"
		<< "</li>\n";
}

static void setIndexNames(struct dirent *dir, std::string &anchorName, std::string &displayName)
{
	anchorName = dir->d_name;
	if (dir->d_type == DT_DIR)
		anchorName += "/";
	displayName = anchorName;
	if (displayName.length() > 25)
		displayName = anchorName.substr(0,22) + "..>";
}

t_httpCode Response::tryAutoIndex(Parameters &p)
{
	if (p.location_.getAutoIndex())
		return FORBIDDEN;
	DIR *d;
	struct dirent *dir;
	std::string anchorName;
	std::string displayName;
	std::ostringstream html;

	d = opendir(p.targetPath_.c_str());
	if (d)
	{
		startHTML(html, p.targetPath_.c_str());
		dir = readdir(d);
		while (dir != NULL)
		{
			// if (dir->d_type == DT_REG || dir->d_type == DT_LNK || dir->d_type == DT_UNKNOWN)
			setIndexNames(dir, anchorName, displayName);
			appendElementToHTML(html, p.targetPath_, anchorName, displayName, dir->d_type);
			dir = readdir(d);
		}
		closedir(d);
		closeHTML(html);
		this->body_.content = html.str();
		this->body_.type = extensionTypesDict_[".html"];
		return OK;
	}

	return FORBIDDEN;
}

t_httpCode Response::methodGet(Parameters &p)
{
	int statCheck;

	statCheck = checkPath(p.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			return getFile(p.targetPath_);
		case S_IFDIR:
			return tryAutoIndex(p);
		case EACCES:
			return FORBIDDEN;

		default:
			return NOT_FOUND;
	}
}

t_httpCode Response::postFile(Parameters &p)
{
	struct stat st;

	if (stat(p.targetPath_.c_str(), &st) == 0)
		return CONFLICT;

	std::ofstream file(p.targetPath_.c_str(), std::ofstream::trunc);
	if (!file.is_open())
	{
		file.close();
        throw (std::runtime_error("Error creating file"));
	}
	file << p.request_.getBody();

	return CREATED;
}

t_httpCode Response::methodPost(Parameters &p)
{
	int statCheck;

	statCheck = checkPath(p.uploadPath_);
	switch (statCheck)
	{
		case S_IFDIR:
			return postFile(p);
		case EACCES:
			return FORBIDDEN;

		default:
			return NOT_FOUND;
	}
}

static t_httpCode removeFile(const char *path)
{
	if (std::remove(path) < 0)
		throw std::runtime_error("Couldn't remove resource");

	return NO_CONTENT;
}

t_httpCode Response::methodDelete(Parameters &p)
{
	int statCheck;

	statCheck = checkPath(p.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			return removeFile(p.targetPath_.c_str());
		case EACCES:
		case S_IFDIR:
			return FORBIDDEN;

		default:
			return NOT_FOUND;
	}
}

t_httpCode Response::executeRequest(Parameters &p)
{
	switch (p.getMethod())
	{
		case M_GET:
			return this->methodGet(p);
		case M_POST:
			return this->methodPost(p);
		case M_DELETE:
			return this->methodDelete(p);
		case M_NOT_ALLOWED:
			return METHOD_NOT_ALLOWED;
		
		default:
			return NOT_IMPLEMENTED;
	}
}

static error_page_t setCustomErrorPage(t_httpCode code, std::set<error_page_t> errorPages, error_page_t &page)
{
	bzero(&page, sizeof(page));

	for (std::set<error_page_t>::iterator ite = errorPages.begin(); ite != errorPages.end(); ++ite)
	{
		if (ite->code == code)
			page = *ite;
	}

	return page;
}

void Response::build(Parameters &params) 
{
	t_httpCode code;
	t_connection mode;
	error_page_t errorPage;

	this->clear();

	code = this->executeRequest(params);
	mode = params.getConnectionMode();

	setCustomErrorPage(code, params.location_.getErrorPages(), errorPage);
	if (errorPage.code)
		this->generateResponse(errorPage, mode);
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
}

Response::~Response()
{
}