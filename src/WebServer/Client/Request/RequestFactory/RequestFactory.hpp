#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static Result<RequestLineParams_t> validateRequestLine(const RequestLineParams_t &requestLine);
    static Result<RequestLineParams_t> getRequestLine(const std::string &requestLineString);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
