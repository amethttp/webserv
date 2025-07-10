#include "Get.hpp"

mGet::mGet()
{
}

static t_httpCode executeCGI(Context &ctx, t_cgi &cgi, t_body &body)
{
	int pipefd[2];
	pid_t child;
	time_t startTime;

	if (pipe(pipefd))
		throw (std::runtime_error("Ceci n'est pas une pipe"));
	startTime = std::time(NULL);
	child = fork();
	if (child < 0)
		throw (std::runtime_error("Couldn't fork CGI properly"));
	else if (child == CHILD_OK)
	{
		char *env[] = { NULL };
		char *argv[] = { strdup(cgi.second.c_str()), strdup(ctx.targetPath_.c_str()), NULL };

		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		execve(cgi.second.c_str(), argv, env);
		exit(1);
	}

	return waitForOutput(child, pipefd, startTime, body);
}

static t_httpCode getFile(std::string &target, t_body &body)
{
	body.content = readFileToString(target.c_str());
	// body.type = getMIME(target);

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

static t_httpCode tryAutoIndex(Context &ctx, t_body &body)
{
	if (!ctx.location_.getAutoIndex())
		return FORBIDDEN;

	DIR *d;
	struct dirent *dir;
	std::string anchorName;
	std::string displayName;
	std::ostringstream html;

	d = opendir(ctx.targetPath_.c_str());
	if (d)
	{
		startHTML(html, ctx.targetPath_.c_str());
		dir = readdir(d);
		while (dir != NULL)
		{
			// if (dir->d_type == DT_REG || dir->d_type == DT_LNK || dir->d_type == DT_UNKNOWN)
			setIndexNames(dir, anchorName, displayName);
			appendElementToHTML(html, ctx.targetPath_, anchorName, displayName, dir->d_type);
			dir = readdir(d);
		}
		closedir(d);
		closeHTML(html);
		body.content = html.str();
		// body.type = this->extensionTypesDict_[".html"];
		return OK;
	}

	return FORBIDDEN;
}

static bool findIndex(Context &ctx)
{
	std::string indexPath;
	std::vector<std::string> indexList = ctx.location_.getIndexList();

	for (std::vector<std::string>::iterator ite = indexList.begin(); ite != indexList.end(); ++ite)
	{
		indexPath = ctx.targetPath_ + (*ite);
		if (checkPath(indexPath) == S_IFREG)
		{
			ctx.targetPath_ = indexPath;
			return true;
		}
	}

	return false;
}

static t_httpCode tryIndex(Context &ctx, t_body &body)
{
	// normalizeTrailingSlash(ctx.targetPath_);
	if (findIndex(ctx))
		return getFile(ctx.targetPath_, body);
	
	return tryAutoIndex(ctx, body);
}

ExecutionResult mGet::execute(Context &ctx)
{
	ExecutionResult res;

	int statCheck;

	statCheck = checkPath(ctx.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			res.code_ = getFile(ctx.targetPath_, res.tempBody_);
			break ;
		case S_IFDIR:
			res.code_ = tryIndex(ctx, res.tempBody_);
			break ;
		case EACCES:
			res.code_ = FORBIDDEN;
			break ;

		default:
			res.code_ = NOT_FOUND;
			break ;
	}

	return res;
}

mGet::~mGet()
{
}
