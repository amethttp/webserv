#include <iostream>
#include "Response.hpp"
#include "RequestExecutor.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <unistd.h>

static void normalizeTrailingSlash(std::string &path)
{
	if (*path.rbegin() != '/')
		path += '/';
}

static bool pathExists(std::string &path)
{
	struct stat st;

	return (stat(path.c_str(), &st) == 0);
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
std::string RequestExecutor::getMIME(std::string &target)
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

void RequestExecutor::setBodyFromString(std::string str)
{
	this->body_.content = str;
	this->body_.type = this->extensionTypesDict_[".txt"];
}

void RequestExecutor::setBodyFromFile(std::string target)
{
	this->body_.content = readFileToString(target.c_str());
	this->body_.type = getMIME(target);
}

t_httpCode RequestExecutor::getFile(std::string &target)
{
	this->setBodyFromFile(target);

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
t_httpCode RequestExecutor::tryAutoIndex(Context &ctx)
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
		this->body_.content = html.str();
		this->body_.type = this->extensionTypesDict_[".html"];
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
t_httpCode RequestExecutor::tryIndex(Context &ctx)
{
	// normalizeTrailingSlash(ctx.targetPath_);
	if (findIndex(ctx))
		return getFile(ctx.targetPath_);
	
	return tryAutoIndex(ctx);
}

t_httpCode RequestExecutor::methodGet(Context &ctx)
{
	int statCheck;

	statCheck = checkPath(ctx.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			return getFile(ctx.targetPath_);
		case S_IFDIR:
			return tryIndex(ctx);
		case EACCES:
			return FORBIDDEN;

		default:
			return NOT_FOUND;
	}
}
t_httpCode RequestExecutor::postFile(Context &ctx)
{
	if (pathExists(ctx.targetPath_))
		return CONFLICT;

	// normalizeTrailingSlash(ctx.targetPath_);
	std::ofstream file(ctx.targetPath_.c_str(), std::ofstream::trunc);
	if (!file.is_open())
	{
		file.close();
		throw (std::runtime_error("Error creating file"));
	}
	file << ctx.request_.getBody();
	this->tempHeaders_["Content-Location"] = ctx.targetPath_;

	return CREATED;
}
t_httpCode RequestExecutor::methodPost(Context &ctx)
{
	int statCheck;

	statCheck = checkPath(ctx.uploadPath_);
	switch (statCheck)
	{
		case S_IFDIR:
			return postFile(ctx);
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
t_httpCode RequestExecutor::methodDelete(Context &ctx)
{
	int statCheck;

	statCheck = checkPath(ctx.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			return removeFile(ctx.targetPath_.c_str());
		case EACCES:
		case S_IFDIR:
			return FORBIDDEN;

		default:
			return NOT_FOUND;
	}
}

t_httpCode RequestExecutor::executeMethod(Context &ctx, ExecutionResult result)
{
	switch (ctx.getMethod()) // move methods into abstract class -> Method.execute...
	{
		case M_GET:
			return this->methodGet(ctx);
		case M_POST:
			return this->methodPost(ctx);
		case M_DELETE:
			return this->methodDelete(ctx);
		case M_NOT_ALLOWED:
			return METHOD_NOT_ALLOWED;
		
		default:
			return NOT_IMPLEMENTED;
	}
}

static bool matchCGI(std::string &path, Location &location, t_cgi &cgi)
{
	int pos = path.rfind('.');
	std::map<std::string, std::string>::iterator cgiIt;
	std::map<std::string, std::string> allowedCGIs = location.getCGIs();

	if (pos != std::string::npos)
	{
		cgiIt = allowedCGIs.find(path.substr(pos));
		if (cgiIt != allowedCGIs.end())
		{
			cgi.first = cgiIt->first;
			cgi.second = cgiIt->second;
			return true;
		}
	}

	return false;
}

static bool timedOut(time_t start)
{
	if ((std::time(NULL) - start) > CGI_TIMEOUT)
		return true;
	return false;
}

static std::string readOutput(int pipefd[2])
{
	std::stringstream output;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

	close(pipefd[1]);
	while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		output.write(buffer, bytes_read);
	close(pipefd[0]);

	return output.str();
}
t_httpCode RequestExecutor::waitForOutput(pid_t child, int pipefd[2], time_t start)
{
	int status = 0;

	while (waitpid(child, &status, WNOHANG) == 0)
	{
		if (timedOut(start))
		{
			kill(child, SIGKILL);
			return GATEWAY_TIME_OUT;
		}
		usleep(100);
	}
	if (status < 0)
		throw (std::runtime_error("WaitPid failed"));
	if (WEXITSTATUS(status) != 0)
		return INTERNAL_SERVER_ERROR;

	this->body_.content = readOutput(pipefd);
	this->body_.type = this->extensionTypesDict_[".txt"];

	return OK;
}

t_httpCode RequestExecutor::executeCGI(Context &ctx, t_cgi &cgi)
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

	return waitForOutput(child, pipefd, startTime);
}

ExecutionResult RequestExecutor::executeRequest(Context &ctx)
{
	t_cgi cgi;
	ExecutionResult result;

	if (matchCGI(ctx.targetPath_, ctx.location_, cgi)) // keep only this here
	{
		if (this->executeCGI(ctx, cgi) == OK) // to each method ...
		{
			// check correctness?? || parse request
			result.isCGI_ = true;
			return OK;
		}
	}

	this->executeMethod(ctx, result);

	return result;
}
