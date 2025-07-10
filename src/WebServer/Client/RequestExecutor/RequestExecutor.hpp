#pragma once

#include <string>
#include <cstdlib>
#include <sys/wait.h>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Response/Response.hpp"
#include "WebServer/Server/Location/Location.hpp"
#include "RequestExecutor/Context/Context.hpp"
#include "RequestExecutor/Result/ExecutionResult.hpp"

#define CHILD_OK 0
#define CGI_TIMEOUT 2
#define BUFFER_SIZE 4096

typedef std::map<std::string, std::string> t_headers;

class RequestExecutor
{
private:
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

public:
	RequestExecutor();
	~RequestExecutor();

	static ExecutionResult executeRequest(Context &c);
};
