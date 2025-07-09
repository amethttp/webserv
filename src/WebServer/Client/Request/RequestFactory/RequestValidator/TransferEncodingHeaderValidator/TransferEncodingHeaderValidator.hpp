#pragma once

#include "WebServer/Client/Request/Request.hpp"

class TransferEncodingHeaderValidator
{
public:
    static bool isValid(const headerValue_t &transferEncodingHeaderValues);
};
