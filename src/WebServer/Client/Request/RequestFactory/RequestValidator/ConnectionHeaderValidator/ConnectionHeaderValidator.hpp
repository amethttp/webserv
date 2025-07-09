#pragma once

#include "WebServer/Client/Request/Request.hpp"

class ConnectionHeaderValidator
{
public:
    static bool isValid(const headerValue_t &connectionHeaderValues);
};
