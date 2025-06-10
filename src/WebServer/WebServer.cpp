#include "WebServer.hpp"
#include <iostream>
#include "Config/ConfigReader/ConfigReader.hpp"
#include "Config/Config.hpp"

void WebServer::setConfigFromFile(std::string path)
{
	ConfigReader::setConfigs(path, this->servers_);
}

void WebServer::serve()
{
	std::cout << "Serving..." << std::endl;
	while (42)
	{
	}
}

WebServer::~WebServer()
{
	// delete this->config_;
}