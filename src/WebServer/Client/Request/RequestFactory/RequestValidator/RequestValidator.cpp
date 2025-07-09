#include "RequestValidator.hpp"
#include "utils/string/string.hpp"
#include "utils/headers/headers.hpp"
#include "HostHeaderValidator/HostHeaderValidator.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"
#include "ContentLengthHeaderValidator/ContentLengthHeaderValidator.hpp"

bool RequestValidator::isValidTransferEncodingHeader(const headerValue_t &transferEncodingHeaderValues)
{
    if (transferEncodingHeaderValues.size() != 1)
        return false;

    const std::string transferEncodingValue = toLower(transferEncodingHeaderValues.front());

    return transferEncodingValue == "chunked";
}

bool RequestValidator::isValidConnectionHeader(const headerValue_t &connectionHeaderValues)
{
    if (connectionHeaderValues.size() != 1)
        return false;

    const std::string connectionHeaderValue = toLower(connectionHeaderValues.front());

    return (connectionHeaderValue == "keep-alive" || connectionHeaderValue == "close");
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
        || !HostHeaderValidator::isValid(requestHeaders.at("Host")))
        return SimpleResult::fail("400 Bad Request");

    if (containsHeader(requestHeaders, "Content-Length")
        && containsHeader(requestHeaders, "Transfer-Encoding"))
        return SimpleResult::fail("400 Bad Request");

    if (containsHeader(requestHeaders, "Content-Length")
        && !ContentLengthHeaderValidator::isValid(requestHeaders.at("Content-Length")))
        return SimpleResult::fail("400 Bad Request");

    if (containsHeader(requestHeaders, "Transfer-Encoding")
        && !isValidTransferEncodingHeader(requestHeaders.at("Transfer-Encoding")))
        return SimpleResult::fail("400 Bad Request");

    if (containsHeader(requestHeaders, "Connection")
        && !isValidConnectionHeader(requestHeaders.at("Connection")))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
