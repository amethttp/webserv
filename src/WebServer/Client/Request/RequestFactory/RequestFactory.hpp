#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"

class RequestFactory
{
private:
    RequestFactory();

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
