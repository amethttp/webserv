#pragma once

#include <string>
#include <vector>
#include "Server/Server.hpp"
#include "Request/Request.hpp"

namespace ServerMatcher
{
    Server matchServer(Request &request, std::vector<Server> &serverList);
}