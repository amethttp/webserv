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

std::string trim(const std::string &input, const std::string &charsToTrim)
{
	size_t start = input.find_first_not_of(charsToTrim);
	if (start == std::string::npos)
		return "";

	size_t end = input.find_last_not_of(charsToTrim);
	return input.substr(start, end - start + 1);
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
