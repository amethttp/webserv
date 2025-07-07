#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestProcesser
{
private:
    RequestProcesser();

    static SimpleResult decodeTargetComponents(Target_t &target);

public:
    static SimpleResult processRequestTarget(Target_t &target);
};
