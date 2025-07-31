#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Connection/Request/Request.hpp"
#include "WebServer/Connection/Request/RequestLine/RequestLine.hpp"

class RequestValidator
{
private:
    RequestValidator();

public:
    static SimpleResult validateRequestLine(const RequestLine &requestLine);
    static SimpleResult validateRequestHeaders(const HeaderCollection &requestHeaders);
};
