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

std::vector<std::string> splitSet(const std::string &input, const std::string &delimiters)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = input.find_first_of(delimiters);

	while (end != std::string::npos)
	{
		if (end > start)
			result.push_back(input.substr(start, end - start));
		start = end + 1;
		end = input.find_first_of(delimiters, start);
	}

	if (start < input.length())
		result.push_back(input.substr(start));

	return result;
}

std::string trim(const std::string &input, const std::string &charsToTrim)
{
	size_t start = input.find_first_not_of(charsToTrim);
	if (start == std::string::npos)
		return "";

	size_t end = input.find_last_not_of(charsToTrim);
	return input.substr(start, end - start + 1);
}

std::string getSpaceValues()
{
	return SPACE_VALUES;
}