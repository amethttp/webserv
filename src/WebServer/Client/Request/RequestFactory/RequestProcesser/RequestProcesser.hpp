#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

    static SimpleResult processTargetPctDecoding(Target_t &target);
    static SimpleResult processHostHeaderPctDecoding(HeaderCollection &headers);

public:
    static SimpleResult processRequestTarget(Target_t &target);
    static SimpleResult processHeadersNew(HeaderCollection &headers);
    static SimpleResult processHeaders(headers_t &headers);
};
