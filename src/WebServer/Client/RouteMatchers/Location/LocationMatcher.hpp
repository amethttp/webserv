#pragma once

#include <string>
#include "Server/Server.hpp"
#include "Request/Request.hpp"
#include "Location/Location.hpp"

namespace LocationMatcher
{
	// Decide on no locations defined on server
	Location matchLocation(Request &request, Server &server);
}