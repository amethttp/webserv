#include "RequestValidator.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

bool RequestValidator::isValidHostHeader(const std::string &header)
{
    const std::string symbols = "%-._~!$&'()*+,;=";

    if (header.empty())
        return false;

    for (size_t i = 0; i < header.length(); i++)
    {
        if (!std::isalnum(header[i]) && symbols.find(header[i]) == std::string::npos)
            return false;
    }
    return true;
}

SimpleResult RequestValidator::validateRequestLine(const RequestLineParams_t &requestLine)
{
    if (requestLine.method == NOT_IMPLEMENTED)
        return SimpleResult::fail("501 Not Implemented");

    if (requestLine.target.uri.length() > MAX_URI_LENGTH)
        return SimpleResult::fail("414 URI Too Long");

    if (requestLine.httpVersion != "HTTP/1.1")
        return SimpleResult::fail("505 HTTP Version Not Supported");

    return SimpleResult::ok();
}

SimpleResult RequestValidator::validateRequestHeaders(const headers_t &requestHeaders)
{
    if (requestHeaders.find("Host") == requestHeaders.end()
        || !isValidHostHeader(requestHeaders.at("Host")))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
