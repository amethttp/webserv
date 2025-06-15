#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"

class RequestParser
{
public:
    SimpleResult parseRequestLine(Request_t &request, const std::string &requestLine);
};
