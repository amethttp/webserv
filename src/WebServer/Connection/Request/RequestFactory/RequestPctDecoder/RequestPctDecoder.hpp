#pragma once

#include "WebServer/Connection/Request/RequestLine/RequestLine.hpp"

class RequestPctDecoder
{
private:
    RequestPctDecoder();

public:
    static std::string decode(const std::string &string);
    static bool isWellEncoded(const std::string &string);
};
