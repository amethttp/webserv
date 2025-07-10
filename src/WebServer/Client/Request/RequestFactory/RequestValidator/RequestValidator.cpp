#include "RequestValidator.hpp"
#include "utils/string/string.hpp"
#include "HostHeaderValidator/HostHeaderValidator.hpp"
#include "ConnectionHeaderValidator/ConnectionHeaderValidator.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"
#include "ContentLengthHeaderValidator/ContentLengthHeaderValidator.hpp"
#include "TransferEncodingHeaderValidator/TransferEncodingHeaderValidator.hpp"

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

SimpleResult RequestValidator::validateRequestHeaders(const HeaderCollection &requestHeaders)
{
    if (!requestHeaders.contains("Host")
    || !HostHeaderValidator::isValid(requestHeaders.getHeader("Host")))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains("Content-Length")
        && requestHeaders.contains("Transfer-Encoding"))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains("Content-Length")
        && !ContentLengthHeaderValidator::isValid(requestHeaders.getHeader("Content-Length")))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains("Transfer-Encoding")
        && !TransferEncodingHeaderValidator::isValid(requestHeaders.getHeader("Transfer-Encoding")))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains("Connection")
        && !ConnectionHeaderValidator::isValid(requestHeaders.getHeader("Connection")))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
