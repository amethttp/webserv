#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestPctDecoder
{
private:
    RequestPctDecoder();

    static bool isValidPctEncoded(char firstDigit, char secondDigit);

public:
    static Result<std::string> decode(const std::string &string);
    static bool isWellEncoded(const std::string &string);
};
