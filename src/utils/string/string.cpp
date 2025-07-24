#include "string.hpp"

bool endsWith(const std::string &str, const std::string &suffix)
{
	if (suffix.length() > str.length())
		return false;

	return str.rfind(suffix) == str.length() - suffix.length();
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

std::string toLower(const std::string &string)
{
	std::string result;

	for (size_t i = 0; i < string.length(); i++)
	{
		result += static_cast<char>(std::tolower(string[i]));
	}

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

void removeDoubleSlashes(std::string &str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
		if (*it == '/' && (it + 1) != str.end() && *(it + 1) == '/')
			it = str.erase(it);
}

// Decide if we normalize slashes or not || Nginx default beaviour doesnt do it
void removeTrailingSlashes(std::string &str)
{
	if (*(str.rbegin()) == '/' && str.length() > 1)
		str.erase(str.length() - 1);
}

int countMatchingDepth(std::string base, std::string target)
{
	size_t i = 0;

	if (base.length() > target.length())
		return -1;

	while (i < base.length())
	{
		if (base[i] != target[i])
			break ;
		i++;
	}

	if (i != base.length())
		return -1;
	else if ((base.length() != target.length()) && (base[i - 1] != '/'))
		return -1;

	return i;
}
