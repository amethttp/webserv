#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestValidator
{
private:
    RequestValidator();

    static bool isValidContentLengthHeader(const headerValue_t &contentLengthHeaderValues);
    static bool isValidTransferEncodingHeader(const headerValue_t &transferEncodingHeaderValues);
    static bool isValidConnectionHeader(const headerValue_t &connectionHeaderValues);

public:
    static SimpleResult validateRequestLine(const RequestLineParams_t &requestLine);
    static SimpleResult validateRequestHeaders(const headers_t &requestHeaders);
};
