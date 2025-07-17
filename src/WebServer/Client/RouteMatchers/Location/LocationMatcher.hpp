#pragma once

#include <string>
#include "WebServer/Server/Server.hpp"
#include "WebServer/Server/Location/Location.hpp"
#include "WebServer/Client/Request/Request.hpp"

namespace LocationMatcher
{
	// Decide on no locations defined on server
	Location *matchLocation(Request &request, Server &server);
}
