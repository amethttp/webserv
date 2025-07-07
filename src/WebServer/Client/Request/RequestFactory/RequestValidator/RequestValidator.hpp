#pragma once
#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestValidator
{
private:
    RequestValidator();

public:
    static bool containsInvalidChars(const std::string &string, const std::string &validChars);
    static SimpleResult validateRequestLine(const RequestLineParams_t &requestLine);
    static SimpleResult validateRequestHeaders(const headers_t &requestHeaders);
};
