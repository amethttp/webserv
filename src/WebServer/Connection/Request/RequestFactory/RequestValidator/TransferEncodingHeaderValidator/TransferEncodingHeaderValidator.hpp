#pragma once

#include "WebServer/Connection/Request/Request.hpp"

class TransferEncodingHeaderValidator
{
public:
    static bool isValid(const Header &transferEncodingHeader);
};
