#pragma once

#include "WebServer/Connection/Request/Request.hpp"

class ConnectionHeaderValidator
{
public:
    static bool isValid(const Header &connectionHeader);
};
