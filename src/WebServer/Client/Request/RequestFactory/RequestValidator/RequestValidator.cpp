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
    if (!requestHeaders.contains(HOST)
    || !HostHeaderValidator::isValid(requestHeaders.getHeader(HOST)))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains(CONTENT_LENGTH)
        && requestHeaders.contains(TRANSFER_ENCODING))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains(CONTENT_LENGTH)
        && !ContentLengthHeaderValidator::isValid(requestHeaders.getHeader(CONTENT_LENGTH)))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains(TRANSFER_ENCODING)
        && !TransferEncodingHeaderValidator::isValid(requestHeaders.getHeader(TRANSFER_ENCODING)))
        return SimpleResult::fail("400 Bad Request");

    if (requestHeaders.contains(CONNECTION)
        && !ConnectionHeaderValidator::isValid(requestHeaders.getHeader(CONNECTION)))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
