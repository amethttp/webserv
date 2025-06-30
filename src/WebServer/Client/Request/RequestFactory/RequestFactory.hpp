#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static void splitRequestTargetComponents(Target_t &target);
    static Result<std::string> decodePath(const std::string &path);
    static bool isEncodedQueryValid(const std::string &query);
    static Result<Target_t> decodeTarget(const Target_t &encodedTarget);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
