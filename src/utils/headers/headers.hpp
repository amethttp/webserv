#pragma once

#include "WebServer/Client/Request/Request.hpp"
#include <string>

void addHeader(headers_t &headers, const std::string &header);

bool containsHeader(const headers_t &headers, const std::string &headerKey);
