#include "utils/fileHandler/files.hpp"
#include "ConfigParser.hpp"



void ConfigParser::useConfig(std::string &path, WebServer &ws)
{
	std::cout << "parsing " << path << std::endl;
	std::string fileExtension = getFileExtension(path);
	if (fileExtension != "conf")
		throw std::runtime_error("Only .conf files are allowed");
	std::string config = readFileToString(path);
	std::cout << "config:\n" << config << std::endl;
	// TODO: Check file acceptance (not empty, contains necessary things, syntax ok)
	// TODO: Fill WebServer from configs
}