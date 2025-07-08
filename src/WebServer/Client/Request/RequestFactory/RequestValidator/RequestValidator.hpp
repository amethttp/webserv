#pragma once
#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestValidator
{
private:
    RequestValidator();

    static bool isUnreserved(char c);
    static bool isPctEncoded(const std::string &header, size_t pos);
    static bool isRegName(const std::string &header, size_t pos);
    static bool isValidHostHeader(const std::string &header);

public:
    static SimpleResult validateRequestLine(const RequestLineParams_t &requestLine);
    static SimpleResult validateRequestHeaders(const headers_t &requestHeaders);
};
