#pragma once

#include "WebServer/Client/Request/Request.hpp"

class ContentLengthHeaderValidator
{
public:
    static bool isValid(const Header &contentLengthHeader);
};
