#include "Get.hpp"
#include "utils/exceptions/Exceptions.hpp"

mGet::mGet()
{
}

static t_httpCode getFile(const std::string &target, t_Body &body)
{
	body.content = readFileToString(target.c_str());
	body.type = getMIME(target);

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

static void appendElementToHTML(std::ostringstream &html, const std::string &targetPath, const std::string &anchorName, const std::string &displayName, unsigned char type)
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
		 << "<div><a href=\"" << anchorName << "\">" << displayName << "</a></div>\n"
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
		displayName = anchorName.substr(0, 22) + "..>";
}

static t_httpCode tryAutoIndex(const Context &ctx, t_Body &body)
{
	if (!ctx.getLocation().getAutoIndex())
		return FORBIDDEN;

	DIR *d;
	struct dirent *dir;
	std::string anchorName;
	std::string displayName;
	std::ostringstream html;

	d = opendir(ctx.getTargetPath().c_str());
	if (d)
	{
		startHTML(html, ctx.getTargetPath().c_str());
		dir = readdir(d);
		while (dir != NULL)
		{
			// if (dir->d_type == DT_REG || dir->d_type == DT_LNK || dir->d_type == DT_UNKNOWN)
			setIndexNames(dir, anchorName, displayName);
			appendElementToHTML(html, ctx.getTargetPath(), anchorName, displayName, dir->d_type);
			dir = readdir(d);
		}
		if (closedir(d))
			throw(RecoverableException("Couldn't close dir"));
		closeHTML(html);
		body.content = html.str();
		body.type = Connection::getExtensionType(".html");
		return OK;
	}

	return FORBIDDEN;
}

static bool findIndex(const Context &ctx)
{
	std::string indexPath;
	std::vector<std::string> indexList = ctx.getLocation().getIndexList();

	for (std::vector<std::string>::iterator ite = indexList.begin(); ite != indexList.end(); ++ite)
	{
		indexPath = ctx.getTargetPath() + (*ite);
		if (checkPath(indexPath) == S_IFREG)
		{
			ctx.getTargetPath() = indexPath;
			return true;
		}
	}

	return false;
}

static t_httpCode tryIndex(const Context &ctx, t_Body &body)
{
	// normalizeTrailingSlash(ctx.getTargetPath());
	if (findIndex(ctx))
		return getFile(ctx.getTargetPath(), body);

	return tryAutoIndex(ctx, body);
}

static void run(const Context &ctx, t_HandlingResult &res)
{
	int statCheck;

	statCheck = checkPath(ctx.getTargetPath());
	switch (statCheck)
	{
	case S_IFREG:
		res.code_ = getFile(ctx.getTargetPath(), res.tempBody_);
		break;
	case S_IFDIR:
		res.code_ = tryIndex(ctx, res.tempBody_);
		break;
	case EACCES:
		res.code_ = FORBIDDEN;
		break;

	default:
		res.code_ = NOT_FOUND;
		break;
	}
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
	const std::string scriptName = "SCRIPT_NAME=" + ctx.getRequest().requestLine.getTargetPath();
	const std::string queryString = "QUERY_STRING=" + ctx.getRequest().requestLine.getTargetQuery();
	const std::string serverName = "SERVER_NAME=" + getHostName(ctx.getRequest().headers.getHeaderValue(HOST));
	const std::string serverPort = "SERVER_PORT=" + getHostPort(ctx.getRequest().headers.getHeaderValue(HOST));
	const std::string cookie = "HTTP_COOKIE=" + ctx.getRequest().headers.getHeaderValue("Cookie");

	size_t envSize = 8;
	char **env = (char **)malloc(sizeof(char *) * (envSize + 1));
	if (!env)
		return NULL;

	env[0] = strdup("REQUEST_METHOD=POST");
	env[1] = strdup(scriptName.c_str());
	env[2] = strdup(queryString.c_str());
	env[3] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	env[4] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[5] = strdup(serverName.c_str());
	env[6] = strdup(serverPort.c_str());
	env[7] = strdup(cookie.c_str());
	env[envSize] = NULL;

	return env;
}

static char **setArgs(const Context &ctx, t_cgi &cgi)
{
	size_t argvSize = 2;

	char **argv = (char **)malloc(sizeof(char *) * (argvSize + 1));
	if (!argv)
		return NULL;

	argv[0] = strdup(cgi.second.c_str());
	argv[1] = strdup(ctx.getTargetPath().c_str());
	argv[argvSize] = NULL;

	return argv;
}

static t_httpCode handleCgiOutput(const Context &ctx, t_cgi &cgi, t_Body &body)
{
	int pipefd[2];
	pid_t child;
	time_t startTime;

	if (pipe(pipefd))
		throw(RecoverableException("Ceci n'est pas une pipe"));
	startTime = std::time(NULL);
	child = fork();
	if (child < 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		throw(RecoverableException("Couldn't fork CGI properly"));
	}
	else if (child == CHILD_OK)
	{
		char **env = setEnvironment(ctx);
		char **argv = setArgs(ctx, cgi);

		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		execve(cgi.second.c_str(), argv, env);
		freeArr(env);
		freeArr(argv);
		exit(1);
	}

	return waitForOutput(child, pipefd, startTime, body);
}

static void runCGI(const Context &ctx, t_cgi &cgi, t_HandlingResult &res)
{
	res.code_ = handleCgiOutput(ctx, cgi, res.tempBody_);
	if (res.code_ == OK)
		res.isCGI_ = true;
	else if (res.code_ == GATEWAY_TIME_OUT)
		res.mode_ = C_CLOSE;
}

t_HandlingResult mGet::execute(const Context &ctx)
{
	t_cgi cgi;
	t_HandlingResult res;

	res.mode_ = ctx.getConnectionMode();
	if (matchCGI(ctx.getTargetPath(), ctx.getLocation(), cgi))
		runCGI(ctx, cgi, res);
	else
		run(ctx, res);

	return res;
}

mGet::~mGet()
{
}
