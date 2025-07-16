#pragma once

#include "WebServer/Client/Request/RequestLine/RequestLine.hpp"

class RequestTargetSeparator
{
private:
    RequestTargetSeparator();

public:
    static void separateComponents(Target_t &target);
};
