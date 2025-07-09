#pragma once

#include <string>
#include <cstdlib>
#include <sys/wait.h>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Location/Location.hpp"
#include "RequestExecutor/Context/Context.hpp"

#define INDEX_STYLE "src/utils/htmlTemplates/indexStyle.html"
#define INDEX_FILE_LIST "src/utils/htmlTemplates/indexFileList.html"
#define INDEX_CLOSE "src/utils/htmlTemplates/closeIndex.html"

#define CHILD_OK 0
#define CGI_TIMEOUT 2
#define BUFFER_SIZE 4096

typedef std::pair<std::string, std::string> t_cgi;
typedef std::map<std::string, std::string> t_headers;

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
	t_headers headers_; // make response a dto/simpler class
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

	t_httpCode tryIndex(Context &c);
	t_httpCode tryAutoIndex(Context &c);
	t_httpCode getFile(std::string &target);
	t_httpCode postFile(Context &c);
	t_httpCode methodGet(Context &c);
	t_httpCode methodPost(Context &c);
	t_httpCode methodDelete(Context &c);
	t_httpCode executeMethod(Context &c);
	t_httpCode executeCGI(Context &c, t_cgi &cgi);
	t_httpCode waitForOutput(pid_t child, int pipefd[2], time_t start);

	void executeRequest(Context &c);
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

	void build(Context &responseParams);
	void build(t_httpCode code, t_connection mode);
};
