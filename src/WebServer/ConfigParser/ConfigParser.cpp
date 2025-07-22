#include "utils/fileHandler/files.hpp"
#include "ConfigParser.hpp"
#include "models/configs.hpp"
#include "utils/string/string.hpp"
#include "helpers/DirectiveRegistry/DirectiveRegistry.hpp"

static void checkSyntax(t_configs &configs)
{
	// TODO: Each directive has required values
	// TODO: Each container has required values
	// TODO: Keywords are known
	// TODO: Check semi-colon ends the directive' line
	std::vector<std::string> lines = split(configs.fileContent, "\n");
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::cout << "line " << i << ": " << lines[i] << std::endl;
		std::vector<std::string> words = splitSet(lines[i], SPACE_VALUES);
		std::string jumpValues = "{};#";
		if (words.size() == 0 || words[0].empty() || jumpValues.find(words[0][0]) < jumpValues.length())
			continue;
		SDirective directive;
		try
		{
			directive = DirectiveRegistry::getDirective(words[0]);
		}
		catch (const std::exception &e)
		{
			std::stringstream what;
			what << "Unknown directive \"" << words[0] << "\" at line " << i + 1;
			throw std::runtime_error(what.str());
		}
		std::cout << "Found directive (" << words[0] << ") --> argMin: " << directive.argMin
				  << "; argMax: " << directive.argMax
				  << "; isContainer: " << directive.isContainer
				  << std::endl;
		std::stringstream what;
		what << "Directive " << words[0] << " expected ";
		if (words.size() - 1 < directive.argMin)
		{
			what << "minimum argument count of " << directive.argMin << " and was given " << words.size() - 1;
			throw std::runtime_error(what.str());
		}
		else if (words.size() - 1 > directive.argMax)
		{
			what << "maximum argument count of " << directive.argMin << " and was given " << words.size() - 1;
			throw std::runtime_error(what.str());
		}
	}
	// configs.fileContent
	// TODO: Check all braces are closed
	int braces = 0;
	for (size_t i = 0; i < configs.fileContent.length(); i++)
	{
		if (configs.fileContent[i] == '{')
			braces++;
		else if (configs.fileContent[i] == '}')
			braces--;
	}
	if (braces > 0)
		throw std::runtime_error("Missing some brace to close container");
	else if (braces < 0)
		throw std::runtime_error("Extra closing brace");
}

static void checkFileAcceptance(t_configs &configs)
{
	if (configs.fileContent.length() < 2)
		throw std::runtime_error("Config file is empty");
	checkSyntax(configs);
	// TODO: Check file acceptance (contains necessary things)
}

static void parseConfigs(t_configs &configs)
{
	// TODO: Fill WebServer from configs (line per line?)
	// TODO: Read container directives and store container' containers(general context could be a container)
	// TODO: Inherit parent directives and override them with read of saved containers' directives
}

static void checkFileExtension(t_configs &configs)
{
	std::string fileExtension = getFileExtension(*configs.path);
	if (fileExtension != "conf" && fileExtension != "")
		throw std::runtime_error("Only .conf files are allowed");
}

void ConfigParser::useConfig(std::string &path, WebServer &ws)
{
	std::cout << "parsing " << path << std::endl;
	t_configs configs;
	configs.path = &path;
	configs.ws = &ws;
	checkFileExtension(configs);
	configs.fileContent = readFileToString(path);
	checkFileAcceptance(configs);
	parseConfigs(configs);
	std::cout << "CONFIGS READ:\n"
			  << configs.fileContent << std::endl;
}