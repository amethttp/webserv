#include <sys/socket.h>
#include <exception>
#include <sstream>
#include "DirectiveRegistry.hpp"
#include "utils/http.hpp"

static std::map<std::string, SDirective> initDirectivesMap()
{
    std::map<std::string, SDirective> directives;
    directives["server"] = SDirective(0, 0, true);
    directives["server_name"] = SDirective(1, -1, false);
    directives["listen"] = SDirective(1, SOMAXCONN, false);
    directives["index"] = SDirective(1, -1, false);
    directives["autoindex"] = SDirective(1, 1, false);
    directives["root"] = SDirective(1, 1, false);
    directives["error_page"] = SDirective(2, 2, false);
    directives["client_max_body_size"] = SDirective(1, 1, false);
    directives["method"] = SDirective(1, M_AMOUNT, false);
    directives["upload_path"] = SDirective(1, 1, false);
    directives["cgi"] = SDirective(2, 2, false);
    directives["location"] = SDirective(1, 1, true);
    directives["return"] = SDirective(2, 2, false);
    return directives;
}

std::map<std::string, SDirective> DirectiveRegistry::directives_ = initDirectivesMap();

std::map<std::string, SDirective> DirectiveRegistry::getDirectives()
{
    return directives_;
}

SDirective DirectiveRegistry::getDirective(std::string &key)
{
    SDirective directive;
    try
    {
        directive = directives_.at(key);
    }
    catch (const std::exception &e)
    {
        std::stringstream what;
        what << "Unknown directive \"" << key << "\"";
        throw std::runtime_error(what.str());
    }
    return directive;
}

void DirectiveRegistry::checkConfigNode(ConfigNode &node)
{
    SDirective directive = getDirective(node.name);
    if (node.params.size() > directive.argMax)
    {
        std::stringstream what;
        what << "directive \"" << node.name << "\" expected a maximum amount of " << directive.argMax << " arguments while " << node.params.size() << " were given";
        throw std::runtime_error(what.str());
    }
    else if (node.params.size() < directive.argMin)
    {
        std::stringstream what;
        what << "directive \"" << node.name << "\" expected a minimum amount of " << directive.argMin << " arguments while " << node.params.size() << " were given";
        throw std::runtime_error(what.str());
    }
}
