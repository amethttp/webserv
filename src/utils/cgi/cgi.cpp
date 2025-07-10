#include "cgi.hpp"

bool matchCGI(std::string &path, Location &location, t_cgi &cgi)
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

bool timedOut(time_t start)
{
	if ((std::time(NULL) - start) > CGI_TIMEOUT)
		return true;
	return false;
}

std::string readOutput(int pipefd[2])
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

t_httpCode waitForOutput(pid_t child, int pipefd[2], time_t start, t_body &body)
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

	body.content = readOutput(pipefd);
	body.type = "text/plain";

	return OK;
}
