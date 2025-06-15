#pragma once

#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestInfo/RequestInfo.hpp"

class RequestFactory
{
private:
    RequestFactory();

public:
    static Result<RequestInfo_t> create(const std::string &requestBuffer);
};
