#include <stack>
#include "utils/fileHandler/files.hpp"
#include "ConfigParser.hpp"
#include "models/configs.hpp"
#include "models/ConfigNode.hpp"
#include "models/ConfigNode.hpp"
#include "models/Token.hpp"
#include "utils/string/string.hpp"
#include "helpers/DirectiveRegistry/DirectiveRegistry.hpp"
#include "models/ConfigBlock.hpp"

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

// static void parseConfigs(t_configs &configs)
// {
// TODO: Fill WebServer from configs (line per line?)
// TODO: Read container directives and store container' containers(general context could be a container)
// TODO: Inherit parent directives and override them with read of saved containers' directives
// std::vector<std::string> lines = split(configs.fileContent, "\n");
// std::string line = lines.at(0);
// for (size_t i = 0; i < lines.size(); i++)
// {
// 	std::string token;
// 	bool comment = false;
// 	for (size_t i = 0; i < line.length(); i++)
// 	{
// 		if (comment)
// 			break;
// 		char c = line.at(i);
// 		if (c == '#')
// 			comment = true;
// 		token += c;
// 	}

// 	if (i - 1 < lines.size())
// 		line += lines.at(i + 1);
// }
// }

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

static void fillBlockFromNode(const ConfigNode &node, ConfigBlock &block)
{
	if (node.name == "server_name")
		block.names_ = node.params;
	else if (node.name == "listen")
	{
		for (std::vector<std::string>::const_iterator it = node.params.begin(); it != node.params.end(); ++it)
		{
			int port;
			if (std::istringstream(*it) >> port)
				block.ports_.push_back(port);
		}
	}
	else if (node.name == "upload_path")
		block.uploadPath_ = node.params.at(0);
	else if (node.name == "root")
		block.root_ = node.params.at(0);
	else if (node.name == "cgi")
		block.cgis_[node.params.at(0)] = node.params.at(1);
	else if (node.name == "autoindex")
		block.autoIndex_ = node.params.at(0) == "on";
	else if (node.name == "index")
		block.indexList_ = node.params;
	else if (node.name == "client_max_body_size")
	{
		// TODO: Split magnitud and check if an integer is sufficient
		int val;
		if (std::istringstream(node.params.at(0)) >> val)
			block.clientMaxBodySize_ = val;
	}
	else if (node.name == "method")
	{
		for (size_t i = 0; i < node.params.size(); i++)
		{
			t_method method = getHTTPMethod(node.params.at(i));
			if (method != METHOD_NOT_ALLOWED)
				block.methods_.insert(method);
		}
	}
	else if (node.name == "error_page")
	{
		t_error_page err;
		int num;
		if (std::istringstream(node.params.at(0)) >> num)
			err.code = (t_httpCode)num;
		err.page = node.params.at(1);
		block.errorPages_.insert(err);
	}
	else if (node.name == "return")
	{
		int num;
		if (std::istringstream(node.params.at(0)) >> num)
			block.return_.code = (t_httpCode)num;
		block.return_.path = node.params.at(1);
	}
}

static void mapTreeToBlock(const std::vector<ConfigNode> &tree, ConfigBlock &block)
{
	// std::cout << "Entro once" << std::endl;
	std::vector<ConfigNode> pendingChildren;
	for (std::vector<ConfigNode>::const_iterator it = tree.begin(); it != tree.end(); ++it)
	{
		const ConfigNode &node = *it;
		if (node.name == "server" || node.name == "location")
			pendingChildren.push_back(node);
		else
			fillBlockFromNode(node, block);
	}
	// std::cout << block << std::endl;
	for (std::vector<ConfigNode>::iterator it = pendingChildren.begin(); it != pendingChildren.end(); ++it)
	{
		// std::cout << "Pending size" << pendingChildren.size() << std::endl;
		block.children.push_back(block);
		if (it->name == "server")
			block.children.back().type = SERVER_BLOCK;
		else if (it->name == "location")
		{
			block.children.back().path_ = it->params.at(0);
			block.children.back().type = LOCATION_BLOCK;
		}
		mapTreeToBlock(it->children, block.children.back());
	}
}

static void setLocationFromBlock(Location *location, const ConfigBlock &block)
{
	location->setPath(block.path_);
	location->setRoot(block.root_);
	location->setCGIs(block.cgis_);
	location->setAutoIndex(block.autoIndex_);
	location->setIndexList(block.indexList_);
	location->setMaxBodySize(block.clientMaxBodySize_);
	location->setMethods(block.methods_);
	location->setReturn(block.return_);
	location->setErrorPages(block.errorPages_);
}

static void setServerFromBlock(Server *server, const ConfigBlock &block)
{
	server->setPorts(block.ports_);
	server->setNames(block.names_);
	server->setUploadPath(block.uploadPath_);
}

static void fillWebserver(WebServer &webServer, std::vector<ConfigNode> &tree)
{
	ConfigNode &prev = *tree.begin();
	std::vector<Server *> servers;
	std::vector<ConfigNode> parents;
	ConfigBlock block;
	block.clientMaxBodySize_ = 0;
	block.return_.code = (t_httpCode)0;
	block.return_.path = "";
	block.type = CONTEXT_BLOCK;
	mapTreeToBlock(tree, block);
	for (std::vector<ConfigBlock>::iterator it = block.children.begin(); it != block.children.end(); ++it)
	{
		ConfigBlock &children = *it;
		// std::cout << children.type << std::endl;
		if (children.type == SERVER_BLOCK)
		{
			servers.push_back(new Server());
			setServerFromBlock(servers.back(), children);
			std::vector<Location *> tempLocations;
			for (std::vector<ConfigBlock>::iterator it = children.children.begin(); it != children.children.end(); ++it)
			{
				ConfigBlock &nested = *it;
				if (nested.type == LOCATION_BLOCK)
				{
					tempLocations.push_back(new Location());
					setLocationFromBlock(tempLocations.back(), nested);
				}
			}
			servers.back()->setLocations(tempLocations);
		}
	}
	webServer.setServers(servers);
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
	fillWebserver(ws, tree);
	std::cout << ws << std::endl;
}