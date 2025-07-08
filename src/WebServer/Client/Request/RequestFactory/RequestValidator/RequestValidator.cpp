#include "RequestValidator.hpp"
#include "utils/headers/headers.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

bool RequestValidator::isUnreserved(const char c)
{
    const std::string unreservedSymbols = "-._~";

    return (std::isalnum(c) || unreservedSymbols.find(c) != std::string::npos);
}

bool RequestValidator::isRegName(const char c)
{
    const std::string symbols = "%!$&'()*+,;=";

    return (isUnreserved(c) || symbols.find(c) != std::string::npos);
}

bool RequestValidator::isValidHostHeader(const std::string &header)
{
    if (header.empty())
        return false;

    for (size_t i = 0; i < header.length(); i++)
    {
        if (!isRegName(header[i]))
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
    if (!containsHeader(requestHeaders, "Host")
        || !isValidHostHeader(requestHeaders.at("Host")))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
