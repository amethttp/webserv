#pragma once

#include <string>
#include <vector>
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"

namespace ServerMatcher
{
    Server matchServer(Request &request, std::vector<Server> &serverList);
}
