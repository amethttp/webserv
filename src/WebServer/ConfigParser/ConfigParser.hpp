#pragma once
#include <string>
#include "WebServer/WebServer.hpp"

class ConfigParser
{
public:
	virtual ~ConfigParser();
	static void useConfig(std::string &path, WebServer &ws);

private:
};
