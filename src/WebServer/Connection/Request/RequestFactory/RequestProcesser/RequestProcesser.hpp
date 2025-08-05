#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Connection/Request/Request.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

public:
    static void processHeaders(HeaderCollection &headers);
};
