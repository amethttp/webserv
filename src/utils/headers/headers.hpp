#pragma once

#include "WebServer/Client/Request/Request.hpp"
#include <string>

void addHeader(headers_t &headers, const std::string &header);

bool containsHeader(const headers_t &headers, const std::string &headerKey);

std::string getHeader(const headers_t &headers, const std::string &headerKey);

void updateHeader(headers_t &headers, const std::string &headerKey, const std::string &newValue);
