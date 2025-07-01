#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static RequestParser createParser(const std::string &text);
    static SimpleResult validateRequestLine(const RequestLineParams_t &requestLine);
    static Result<RequestLineParams_t> buildRequestLineFromString(const std::string &requestLineString);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
