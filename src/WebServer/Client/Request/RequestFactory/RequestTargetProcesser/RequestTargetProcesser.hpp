#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestTargetProcesser
{
private:
    RequestTargetProcesser();

public:
    static Result<Target_t> process(Target_t &target);
};
