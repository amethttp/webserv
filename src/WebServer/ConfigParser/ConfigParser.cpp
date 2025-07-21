#include "utils/fileHandler/files.hpp"
#include "ConfigParser.hpp"
#include "models/configs.hpp"

static void checkFileAcceptance(t_configs &configs)
{
	if (configs.fileContent.length() < 2)
		throw std::runtime_error("Config file is empty");
	// TODO: Check file acceptance (not empty, contains necessary things, syntax ok)
}

static void parseConfigs(t_configs &configs)
{
	// TODO: Fill WebServer from configs (line per line?)
}

void ConfigParser::useConfig(std::string &path, WebServer &ws)
{
	std::cout << "parsing " << path << std::endl;
	t_configs configs;
	configs.path = &path;
	configs.ws = &ws;
	std::string fileExtension = getFileExtension(path);
	if (fileExtension != "conf" && fileExtension != "")
		throw std::runtime_error("Only .conf files are allowed");
	configs.fileContent = readFileToString(path);
	checkFileAcceptance(configs);
	parseConfigs(configs);
	std::cout << "CONFIGS READ:\n" << configs.fileContent << std::endl;
}