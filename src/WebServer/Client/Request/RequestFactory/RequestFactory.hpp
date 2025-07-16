#pragma once

#include "utils/Result/Result.hpp"
#include "../Request.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

class RequestFactory
{
private:
    RequestFactory();

    static std::string getRequestLineString(const std::string &requestBuffer);
    static std::string getRequestHeadersString(const std::string &requestBuffer);
    static std::string getRequestBodyString(const std::string &requestBuffer);
    static RequestParser createParser(const std::string &text);
    static Result<RequestLine> buildRequestLineFromString(const std::string &requestLineString);
    static Result<HeaderCollection> buildRequestHeadersFromString(const std::string &headersString);
    static Result<Body> buildFullBodyFromString(const size_t &contentLengthSize, const std::string &bodyString);
    static Result<Body> buildChunkedBodyFromString(const std::string &bodyString);
    static Result<Body> buildRequestBodyFromString(const HeaderCollection &headers, const std::string &bodyString);

public:
    static Result<Request_t> create(const std::string &requestBuffer);
};
