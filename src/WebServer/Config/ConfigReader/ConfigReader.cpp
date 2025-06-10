#include "ConfigReader.hpp"
#include "../Config.hpp"

#include <vector>
#include <iostream>
#include <fstream>

static void checkExtension(const std::string &path)
{
	std::string extension;
	try
	{
		extension = path.substr(path.rfind("."));
	}
	catch (const std::exception &e)
	{
	}
	if (extension != ".conf")
	{
		throw std::runtime_error("file \"" + path + "\" has not a valid extension");
	}
}

static bool readWords(std::vector<Server> &servers, std::vector<Config *> context, std::vector<std::string> words)
{

	if (words[0] == "server")
	{
		Server server;
		// servers.push_back(server);
		// context.push_back(&server);
		return true;
	}
	else if (words[0] == "location")
	{
		Location location;
		// (*servers.end()).addLocation(location);
		// context.push_back(&location);
		return true;
	}
	else
	{
		(*context.end())->setConfig(words);
		return false;
	}
	// for (std::vector<std::string>::iterator it = words.begin(); it < words.end(); ++it)
	// {
	// 	std::cout << *it << std::endl;
	// }
}

static void readLine(std::vector<Server> &servers, std::vector<Config *> context, const std::string &line)
{
	size_t i = 0;
	std::vector<std::string> words;
	std::string currRead;
	bool expectedScope;

	while (i < line.length())
	{
		if (line[i] == '#')
			break;
		else if (expectedScope && (line[i] != '{' && !std::isspace(line[i])))
		{
			throw std::runtime_error("expected scope opening");
		}
		else if (line[i] == ';' || line[i] == ' ')
		{
			words.push_back(currRead);
			currRead = "";
			expectedScope = readWords(servers, context, words);
			words.erase(words.begin(), words.end());
		}
		else if (line[i] == '{')
		{
			
		}
		else if (line[i] == '}')
			context.erase(context.end());
		else if (!std::isspace(line[i]))
			currRead.push_back(line[i]);
		i++;
	}
}

static void readFile(const std::string &path, std::vector<Server> &servers)
{
	std::cout << "Reading" << std::endl;
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("file could not be opened");
	std::vector<Config *> context;
	std::string line;
	size_t i = 0;
	while (std::getline(file, line))
		readLine(servers, context, line);
	if (file.bad())
		throw std::runtime_error("I/O error while reading");
	else if (!file.eof())
		throw std::runtime_error("unknown error while reading");
}

void ConfigReader::setConfigs(const std::string &path, std::vector<Server> &servers)
{
	checkExtension(path);
	readFile(path, servers);
	// TODO: Check for overwrites
}