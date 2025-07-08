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
    static bool isSubDelim(char c);
    static bool isRegName(const std::string &header, size_t pos);

    static bool isValidHostName(const std::string &hostName);
    static bool isValidHostPort(const std::string &port);

    static bool isValidHostHeader(const headerValue_t &hostHeaderValues);
    static bool isValidContentLengthHeader(const headerValue_t &contentLengthHeaderValues);
    static bool isValidTransferEncodingHeader(const headerValue_t &transferEncodingHeaderValues);

public:
    static SimpleResult validateRequestLine(const RequestLineParams_t &requestLine);
    static SimpleResult validateRequestHeaders(const headers_t &requestHeaders);
};
