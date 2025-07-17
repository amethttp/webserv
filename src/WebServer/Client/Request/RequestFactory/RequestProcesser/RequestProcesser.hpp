#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLine/RequestLine.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

    static SimpleResult processRequestLinePctDecoding(RequestLine &requestLine);
    static SimpleResult processHostHeaderPctDecoding(HeaderCollection &headers);

public:
    static SimpleResult processRequestLine(RequestLine &requestLine);
    static SimpleResult processHeaders(HeaderCollection &headers);
};
