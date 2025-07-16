#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLine/RequestLine.hpp"

class RequestValidator
{
private:
    RequestValidator();

public:
    static SimpleResult validateRequestLine(const RequestLine &requestLine);
    static SimpleResult validateRequestHeaders(const HeaderCollection &requestHeaders);
};
