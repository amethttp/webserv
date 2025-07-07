#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static RequestParser createParser(const std::string &text);
    static Result<RequestLineParams_t> buildRequestLineFromString(const std::string &requestLineString);
    static Result<headers_t> buildRequestHeadersFromString(const std::string &headersString);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
