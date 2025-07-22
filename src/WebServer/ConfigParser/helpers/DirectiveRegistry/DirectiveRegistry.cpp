#include "DirectiveRegistry.hpp"
#include <sys/socket.h>
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
    return directives_.at(key);
}
