#include "RequestValidator.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

bool RequestValidator::containsInvalidChars(const std::string &string, const std::string &validChars)
{
    for (size_t i = 0; i < string.length(); i++)
    {
        if (!std::isalnum(string[i]) && validChars.find(string[i]) == std::string::npos)
            return true;
    }
    return false;
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
        || requestHeaders.at("Host").empty()
        || containsInvalidChars(requestHeaders.at("Host"), "%-._~!$&'()*+,;="))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
