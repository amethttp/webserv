#include "WebServer.hpp"
#include <iostream>
#include "Config/ConfigReader/ConfigReader.hpp"

void WebServer::setConfigFromFile(std::string path)
{
	try
	{
		ConfigReader::getConfigs(path);
	}
	catch (std::exception e)
	{
		std::cout << "Couldn't read config file <<"
				  << path << ">> because " << e.what()
				  << std::endl;
	}
	// path.rfind(".");
}

void WebServer::serve()
{
	std::cout << "Serving..." << std::endl;
	while (42)
	{
	}
}