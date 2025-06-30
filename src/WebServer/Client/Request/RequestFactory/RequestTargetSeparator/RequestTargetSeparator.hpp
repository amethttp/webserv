#pragma once

#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestTargetSeparator
{
private:
    RequestTargetSeparator();

public:
    static void separateInComponents(Target_t &target);
};
