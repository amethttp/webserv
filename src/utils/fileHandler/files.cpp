#include "files.hpp"

std::string getMIME(std::string &target)
{
	int pos;
	std::string res = "";

	pos = target.rfind('.');
	if (pos != std::string::npos)
		res = Client::getExtensionType(target.substr(pos));
	
	return (res.empty() ? "text/plain" : res);
}

void normalizeTrailingSlash(std::string &path)
{
	if (*path.rbegin() != '/')
		path += '/';
}

bool pathExists(std::string &path)
{
	struct stat st;

	return (stat(path.c_str(), &st) == 0);
}

int checkPath(std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return errno;
	if (access(path.c_str(), R_OK))
		return EACCES;

	return (st.st_mode & S_IFMT);
}

std::string readFileToString(const std::string &path)
{
	struct stat st;
	std::string str;
	std::ifstream file(path.c_str(), std::ifstream::binary);

    if (!file.is_open())
	{
		file.close();
        throw (std::runtime_error("Couldn't open file"));
	}
	if (stat(path.c_str(), &st))
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
