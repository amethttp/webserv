#pragma once
#include "../Config.hpp"
#include "WebServer/Server/Server.hpp"

class ConfigReader
{
private:
public:
	static void setConfigs(const std::string &path, std::vector<Server> &servers);
};