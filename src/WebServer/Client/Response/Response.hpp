#pragma once

#include <string>
#include <sys/wait.h>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "Parameters/Parameters.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Location/Location.hpp"

#define INDEX_STYLE "src/utils/htmlTemplates/indexStyle.html"
#define INDEX_FILE_LIST "src/utils/htmlTemplates/indexFileList.html"
#define INDEX_CLOSE "src/utils/htmlTemplates/closeIndex.html"

#define CHILD_OK 0
#define CGI_TIMEOUT 5000
#define BUFFER_SIZE 4096

typedef struct s_body
{
	std::string content;
	std::string type;
} t_body;

class Response
{
private:
	static std::map<t_httpCode, std::string> errorDict_; // to client
	static std::map<std::string, std::string> extensionTypesDict_; // to client

	std::string buffer_; // to client

	StatusLine statusLine_;
	std::map<std::string, std::string> headers_; // make response a dto/simpler class
	t_body body_;

	// to response generator
	void setStatusLine(t_httpCode code);
	void setResponseHeaders(t_connection mode);
	void setRepresentationHeaders();
	void setRedirectionResponse(t_httpCode code, std::string uri);
	void setBodyFromFile(std::string target);
	void setBodyFromString(std::string str);

	std::string getRedirectionHTML(t_httpCode code, std::string &uri);

	void handleReturnDirective(t_return ret, t_connection mode);
	
	void generateResponse(t_httpCode code, t_connection mode);

	// request executor
	std::string getMIME(std::string &target);

	t_httpCode tryIndex(Parameters &p);
	t_httpCode tryAutoIndex(Parameters &p);
	t_httpCode getFile(std::string &target);
	t_httpCode postFile(Parameters &p);
	t_httpCode methodGet(Parameters &p);
	t_httpCode methodPost(Parameters &p);
	t_httpCode methodDelete(Parameters &p);
	t_httpCode executeMethod(Parameters &p);
	t_httpCode executeCGI(Parameters &p, std::pair<std::string, std::string> &cgi);

	void executeRequest(Parameters &p);
public:
	Response();
	~Response();
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(size_t bytesToErase);

	std::string getBuffer() const;
	t_httpCode getStatusCode() const;
	bool getConnection() const;

	void clear();

	void build(Parameters &responseParams);
	void build(t_httpCode code, t_connection mode);
};
