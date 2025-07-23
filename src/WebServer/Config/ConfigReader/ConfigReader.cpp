#include "ConfigReader.hpp"
#include "../Config.hpp"
#include "ReaderContext/ReaderContext.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

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

static void readWords(std::vector<Server> &servers, ReaderContext &context, std::vector<std::string> words)
{
	for (size_t i = 0; i < words.size(); i++)
	{
		std::cout << words[i] << ",";
		if (words[i] == "server")
		{
			Server server;
			context.setExpectedScope(true);
			// servers.push_back(server);
			// context.push_back(&server);
		}
		else if (words[i] == "location")
		{
			Location location;
			context.setExpectedScope(true);
			i++;
			// (*servers.end()).addLocation(location);
			// context.push_back(&location);
		}
		else if (words[i] == "{")
		{
			if (context.getExpectedScope())
			{
				context.incrScopeLevel();
				context.setExpectedScope(false);
			}
			else
				throw std::runtime_error("unexpected scope opening bracket '{'");
		}
		else if (words[i] == "}")
		{
			if (context.decrScopeLevel() == -1)
				throw std::runtime_error("unexpected scope closing bracket '}'");
		}
		else
		{
			if (context.getExpectedScope())
				throw std::runtime_error("expected scope opening bracket '{'");
			// (*context.end())->setConfig(words);
		}
	}
	std::cout << std::endl;
}

static void readLine(std::vector<Server> &servers, ReaderContext &context, const std::string &line)
{
	size_t i = 0;
	std::vector<std::string> words;
	std::string currRead;

	while (i < line.length())
	{
		if (line[i] == '#')
		break;
		else if (!currRead.empty() && (line[i] == ';' || std::isspace(line[i])))
		{
			words.push_back(currRead);
			currRead = "";
		}
		else if (!std::isspace(line[i]))
			currRead.push_back(line[i]);
		i++;
	}
	if (!currRead.empty())
		words.push_back(currRead);
	readWords(servers, context, words);
}

static void readFile(const std::string &path, std::vector<Server> &servers)
{
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("file could not be opened");
	ReaderContext context;
	std::string line;
	size_t i = 0;
	try
	{
		while (std::getline(file, line))
		{
			readLine(servers, context, line);
			i++;
		}
		if (file.bad())
			throw std::runtime_error("I/O error while reading");
		else if (!file.eof())
			throw std::runtime_error("unknown error while reading");
	}
	catch (const std::exception &e)
	{
		std::ostringstream msg;
		msg << e.what()
			<< " in file " << path << ":" << i;
		throw std::runtime_error(msg.str());
	}
}

void ConfigReader::setConfigs(const std::string &path, std::vector<Server> &servers)
{
	checkExtension(path);
	readFile(path, servers);
	// TODO: Check for overwrites
}