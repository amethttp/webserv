#include <stack>
#include "utils/fileHandler/files.hpp"
#include "ConfigParser.hpp"
#include "models/configs.hpp"
#include "models/ConfigNode.hpp"
#include "models/ConfigNode.hpp"
#include "models/Token.hpp"
#include "utils/string/string.hpp"
#include "helpers/DirectiveRegistry/DirectiveRegistry.hpp"

static void checkFileAcceptance(t_configs &configs)
{
	if (configs.fileContent.length() < 2)
		throw std::runtime_error("Config file is empty");
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

static void parseConfigs(t_configs &configs)
{
	// TODO: Fill WebServer from configs (line per line?)
	// TODO: Read container directives and store container' containers(general context could be a container)
	// TODO: Inherit parent directives and override them with read of saved containers' directives
	std::vector<std::string> lines = split(configs.fileContent, "\n");
	std::string line = lines.at(0);
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::string token;
		bool comment = false;
		for (size_t i = 0; i < line.length(); i++)
		{
			if (comment)
				break;
			char c = line.at(i);
			if (c == '#')
				comment = true;
			token += c;
		}

		if (i - 1 < lines.size())
			line += lines.at(i + 1);
	}
}

static void checkFileExtension(t_configs &configs)
{
	std::string fileExtension = getFileExtension(*configs.path);
	if (fileExtension != "conf" && fileExtension != "")
		throw std::runtime_error("Only .conf files are allowed");
}

static std::vector<Token> tokenize(const std::string &input)
{
	std::vector<Token> tokens;
	std::istringstream stream(input);
	std::string line;

	while (std::getline(stream, line))
	{
		std::string token;
		bool in_comment = false;
		for (size_t i = 0; i < line.size(); ++i)
		{
			char c = line[i];

			if (in_comment)
				break;
			if (c == '#')
			{
				in_comment = true;
				continue;
			}

			if (isspace(c))
			{
				if (!token.empty())
				{
					tokens.push_back((Token){TOKEN_WORD, token});
					token.clear();
				}
			}
			else if (c == '{')
			{
				if (!token.empty())
				{
					tokens.push_back((Token){TOKEN_WORD, token});
					token.clear();
				}
				tokens.push_back((Token){TOKEN_LBRACE, "{"});
			}
			else if (c == '}')
			{
				if (!token.empty())
				{
					tokens.push_back((Token){TOKEN_WORD, token});
					token.clear();
				}
				tokens.push_back((Token){TOKEN_RBRACE, "}"});
			}
			else if (c == ';')
			{
				if (!token.empty())
				{
					tokens.push_back((Token){TOKEN_WORD, token});
					token.clear();
				}
				tokens.push_back((Token){TOKEN_SEMICOLON, ";"});
			}
			else
			{
				token += c;
			}
		}
		if (!token.empty())
		{
			tokens.push_back((Token){TOKEN_WORD, token});
		}
	}

	return tokens;
}

void print_tree(const std::vector<ConfigNode> &nodes, int indent = 0)
{
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		for (int j = 0; j < indent; ++j)
			std::cout << "  ";
		std::cout << nodes[i].name;
		for (size_t k = 0; k < nodes[i].params.size(); ++k)
			std::cout << " " << nodes[i].params[k];
		std::cout << "\n";
		print_tree(nodes[i].children, indent + 1);
	}
}

void print_tokens(const std::vector<Token> &tokens)
{
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		const Token &t = tokens[i];
		std::string type;
		switch (t.type)
		{
		case TOKEN_WORD:
			type = "WORD";
			break;
		case TOKEN_LBRACE:
			type = "LBRACE";
			break;
		case TOKEN_RBRACE:
			type = "RBRACE";
			break;
		case TOKEN_SEMICOLON:
			type = "SEMICOLON";
			break;
		default:
			type = "UNKNOWN";
			break;
		}
		std::cout << "[" << type << "] \"" << t.value << "\"" << std::endl;
	}
}

static void parse_config(const std::string &input, std::vector<ConfigNode> &config_tree)
{
	std::vector<Token> tokens = tokenize(input);
	// print_tokens(tokens);
	std::vector<Token>::const_iterator it = tokens.begin();
	std::vector<Token>::const_iterator end = tokens.end();

	std::stack<Frame> stack;
	stack.push((Frame){&config_tree, ConfigNode(), false});

	while (it != end)
	{
		Token tok = *it++;

		if (tok.type == TOKEN_WORD)
		{
			// std::cout << "ENTRY: " << stack.top().current.name << std::endl;
			Frame &frame = stack.top();

			if (!frame.building)
			{
				// std::cout << "Building " << tok.value << std::endl;
				frame.current.name = tok.value;
				frame.current.params.clear();
				frame.current.children.clear();
				frame.building = true;
			}
			else
				frame.current.params.push_back(tok.value);
		}
		else if (tok.type == TOKEN_SEMICOLON)
		{
			if (!stack.top().building)
			{
				std::stringstream what;
				what << "unexpected semicolon near " << stack.top().current.name;
				throw std::runtime_error(what.str());
			}
			DirectiveRegistry::checkConfigNode(stack.top().current);
			stack.top().container->push_back(stack.top().current);
			stack.top().current = ConfigNode();
			stack.top().building = false;
			// std::cout << "SEMICOLON" << " ( " << stack.top().current.name << " ) " << &stack.top() << std::endl;
		}
		else if (tok.type == TOKEN_LBRACE)
		{
			if (!stack.top().building)
			{
				std::stringstream what;
				what << "unexpected left brace \"{\" near " << stack.top().current.name;
				throw std::runtime_error(what.str());
			}
			DirectiveRegistry::checkConfigNode(stack.top().current);
			stack.top().container->push_back(stack.top().current);
			std::vector<ConfigNode> &children = stack.top().container->back().children;
			stack.push((Frame){&children, stack.top().current, false});
			stack.top().current = ConfigNode();
		}
		else if (tok.type == TOKEN_RBRACE)
		{
			// std::cout << "RBRACE " << &stack.top() << std::endl;
			if (stack.size() <= 1)
			{
				std::stringstream what;
				what << "unexpected closing brace \"}\" near " << stack.top().current.name;
				throw std::runtime_error(what.str());
			}
			else if (stack.top().building && !stack.top().current.name.empty())
			{
				std::stringstream what;
				what << "unexpected1 closing brace \"}\" near " << stack.top().current.name;
				throw std::runtime_error(what.str());
			}
			stack.pop();
			stack.top().building = false;
			continue;
		}
	}
	if (stack.size() == 1 && stack.top().building)
	{
		std::stringstream what;
		what << "missing semicolon near " << stack.top().current.name;
		throw std::runtime_error(what.str());
	}
}

void ConfigParser::useConfig(std::string &path, WebServer &ws)
{
	std::cout << "parsing " << path << std::endl;
	t_configs configs;
	configs.path = &path;
	configs.ws = &ws;
	checkFileExtension(configs);
	configs.fileContent = readFileToString(path);
	std::vector<ConfigNode> tree;
	checkFileAcceptance(configs);
	parse_config(configs.fileContent, tree);
	print_tree(tree);
	// parseConfigs(configs);
	// std::cout << "CONFIGS READ:\n"
	// 		  << configs.fileContent << std::endl;
}