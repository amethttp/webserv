#include "string.hpp"

bool isEmpty(const std::string &str)
{
	return str.empty();
}

std::string toLowerString(const std::string &str)
{
	std::string res(str);

	for (std::string::iterator it = res.begin(); it != res.end(); ++it)
	{
		*it = std::tolower(*it);
	}

	return res;
}

std::vector<std::string> split(const std::string &input, const std::string &del)
{
	std::vector<std::string> result;
	size_t pos = 0;
	size_t next = 0;
	size_t delimLen = del.length();

	next = input.find(del, pos);
	while (next != std::string::npos)
	{
		result.push_back(input.substr(pos, next - pos));
		pos = next + delimLen;
		next = input.find(del, pos);
	}

	result.push_back(input.substr(pos));
	return result;
}


