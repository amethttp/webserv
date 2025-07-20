#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/Request.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

public:
    static SimpleResult processHeaders(HeaderCollection &headers);
};
