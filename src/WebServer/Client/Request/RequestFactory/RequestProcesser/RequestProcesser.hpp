#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLine/RequestLine.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

    static SimpleResult processTargetPctDecoding(Target_t &target);
    static SimpleResult processHostHeaderPctDecoding(HeaderCollection &headers);

public:
    static SimpleResult processRequestTargetNew(RequestLine &requestLine);
    static SimpleResult processRequestTarget(Target_t &target);
    static SimpleResult processHeaders(HeaderCollection &headers);
};
