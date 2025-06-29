#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static std::string decodeTarget(const std::string &encodedTarget);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
