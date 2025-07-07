#pragma once

#include "WebServer/Client/Request/Request.hpp"
#include <string>

bool tryAddHeader(headers_t &headers, const std::string &header);
