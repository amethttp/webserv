#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"
#include <vector>

class RequestParser
{
public:
    Result<RequestLineParams_t> parseRequestLine(const std::string &requestLine);
    void parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer);
};
