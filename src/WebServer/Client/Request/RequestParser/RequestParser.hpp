#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"

class RequestParser
{
public:
    static Result<bool> parseRequestLine(Request_t &request, const std::string &requestLine);
};
