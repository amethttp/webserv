#pragma once
#include <map>
#include <string>
#include "../../models/SDirective.hpp"
#include "../../models/ConfigNode.hpp"

class DirectiveRegistry
{
private:
	static std::map<std::string, SDirective> directives_;

public:
	static std::map<std::string, SDirective> getDirectives();
	static SDirective getDirective(std::string &key);
	static void checkConfigNode(ConfigNode &node);
};