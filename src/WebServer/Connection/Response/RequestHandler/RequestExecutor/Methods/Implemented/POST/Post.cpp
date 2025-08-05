#include "Post.hpp"
#include "utils/exceptions/Exceptions.hpp"

mPost::mPost()
{
}

static t_httpCode postFile(const Context &ctx, t_HandlingResult &res)
{
	if (pathExists(ctx.getTargetPath()))
		return CONFLICT;

	// normalizeTrailingSlash(ctx.getTargetPath());
	std::ofstream file(ctx.getTargetPath().c_str(), std::ofstream::trunc);
	if (!file.is_open())
	{
		file.close();
		throw(RecoverableException("Error creating file"));
	}
	file << ctx.getRequest().body.getMessage();
	res.tempHeaders_.addHeader("Content-Location", ctx.getTargetPath());

	return CREATED;
}

static t_HandlingResult run(const Context &ctx, t_HandlingResult &res)
{
	int statCheck;

	res.mode_ = ctx.getConnectionMode();
	statCheck = checkPath(ctx.getUploadPath());
	switch (statCheck)
	{
	case S_IFDIR:
		res.code_ = postFile(ctx, res);
		break;
	case EACCES:
		res.code_ = FORBIDDEN;
		break;

	default:
		res.code_ = NOT_FOUND;
		break;
	}

	return res;
}

static void freeArr(char **arr)
{
	for (size_t i = 0; arr && arr[i]; ++i)
		free(arr[i]);
	free(arr);
}

static std::string getHostName(const std::string &hostValue)
{
	const size_t portSeparator = hostValue.find(':');
	const std::string hostName = hostValue.substr(0, portSeparator);

	return hostName;
}

static std::string getHostPort(const std::string &hostValue)
{
	const size_t portSeparator = hostValue.find(':');
	const std::string hostPort = hostValue.substr(portSeparator + 1);

	return hostPort;
}

static char **setEnvironment(const Context &ctx)
{
	std::stringstream ss;
	ss << ctx.getRequest().body.getMessage().size();

	const std::string scriptName = "SCRIPT_NAME=" + ctx.getRequest().requestLine.getTargetPath();
	const std::string contentLength = "CONTENT_LENGTH=" + ss.str();
	const std::string contentType = "CONTENT_TYPE=" + ctx.getRequest().headers.getHeaderValue(CONTENT_TYPE);
	const std::string queryString = "QUERY_STRING=" + ctx.getRequest().requestLine.getTargetQuery();
	const std::string serverName = "SERVER_NAME=" + getHostName(ctx.getRequest().headers.getHeaderValue(HOST));
	const std::string serverPort = "SERVER_PORT=" + getHostPort(ctx.getRequest().headers.getHeaderValue(HOST));
	const std::string cookie = "HTTP_COOKIE=" + ctx.getRequest().headers.getHeaderValue("Cookie");


	size_t envSize = 10;
	char **env = new char *[envSize + 1];

	env[0] = strdup("REQUEST_METHOD=POST");
	env[1] = strdup(contentLength.c_str());
	env[2] = strdup(contentType.c_str());
	env[3] = strdup(scriptName.c_str());
	env[4] = strdup(queryString.c_str());
	env[5] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	env[6] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[7] = strdup(serverName.c_str());
	env[8] = strdup(serverPort.c_str());
	env[9] = strdup(cookie.c_str());
	env[envSize] = NULL;

	return env;
}

static char **setArgs(const Context &ctx, t_cgi &cgi)
{
	size_t argvSize = 2;

	char **argv = new char *[argvSize + 1];

	argv[0] = strdup(cgi.second.c_str());
	argv[1] = strdup(ctx.getTargetPath().c_str());
	argv[argvSize] = NULL;

	return argv;
}

static t_httpCode handleCgiOutput(const Context &ctx, t_cgi &cgi, t_Body &body)
{
	int outPipe[2];
	int inPipe[2];
	pid_t child;
	time_t startTime;

	if (pipe(outPipe))
		throw(RecoverableException("Ceci n'est pas une pipe"));
	if (pipe(inPipe))
	{
		close(outPipe[0]);
		close(outPipe[1]);
		throw(RecoverableException("Ceci n'est pas une pipe"));
	}
	startTime = std::time(NULL);
	child = fork();
	if (child < 0)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		throw(RecoverableException("Couldn't fork CGI properly"));
	}
	else if (child == CHILD_OK)
	{
		char **env = setEnvironment(ctx);
		char **argv = setArgs(ctx, cgi);

		close(outPipe[0]);
		close(inPipe[1]);

		dup2(outPipe[1], STDOUT_FILENO);
		close(outPipe[1]);
		dup2(inPipe[0], STDIN_FILENO);
		close(inPipe[0]);

		execve(cgi.second.c_str(), argv, env);
		freeArr(env);
		freeArr(argv);
		exit(1);
	}
	close(outPipe[1]);
	close(inPipe[0]);

	const std::string &reqBody = ctx.getRequest().body.getMessage();
	size_t total = 0;
	ssize_t written = 0;

	while (total < reqBody.size())
	{
		written = write(inPipe[1], reqBody.c_str() + total, reqBody.size() - total);
		if (written <= 0)
			break;
		total += written;
	}
	if (written < 0)
		throw RecoverableException("Error when writing to Child's pipe");
	close(inPipe[1]);

	return waitForOutput(child, outPipe, startTime, body);
}

static void runCGI(const Context &ctx, t_cgi &cgi, t_HandlingResult &res)
{
	res.code_ = handleCgiOutput(ctx, cgi, res.tempBody_);
	if (res.code_ == OK)
		res.isCGI_ = true;
	else if (res.code_ == GATEWAY_TIME_OUT)
		res.mode_ = C_CLOSE;
}

t_HandlingResult mPost::execute(const Context &ctx)
{
	t_cgi cgi;
	t_HandlingResult res;

	res.mode_ = ctx.getConnectionMode();
	if (ctx.getUploadPath().empty())
		res.code_ = FORBIDDEN;
	else if (matchCGI(ctx.getTargetPath(), ctx.getLocation(), cgi))
		runCGI(ctx, cgi, res);
	else
		run(ctx, res);

	return res;
}

mPost::~mPost()
{
}
