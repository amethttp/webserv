#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestTargetProcesser
{
private:
    RequestTargetProcesser();

    static SimpleResult decodeTargetComponents(Target_t &target);

public:
    static SimpleResult process(Target_t &target);
};
