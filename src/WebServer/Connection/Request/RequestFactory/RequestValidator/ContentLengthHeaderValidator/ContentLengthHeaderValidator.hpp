#pragma once

#include "WebServer/Connection/Request/Request.hpp"

class ContentLengthHeaderValidator
{
public:
    static bool isValid(const Header &contentLengthHeader);
};
