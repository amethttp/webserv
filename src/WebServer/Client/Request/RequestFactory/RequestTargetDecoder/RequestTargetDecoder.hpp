#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestLineParams/RequestLineParams.hpp"

class RequestTargetDecoder
{
private:
    RequestTargetDecoder();

    static Result<std::string> decodePath(const std::string &path);
    static bool isEncodedQueryValid(const std::string &query);

public:
    static Result<Target_t> decodeTarget(const Target_t &encodedTarget);
};
