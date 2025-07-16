#include "RequestValidator.hpp"
#include "utils/string/string.hpp"
#include "HostHeaderValidator/HostHeaderValidator.hpp"
#include "ConnectionHeaderValidator/ConnectionHeaderValidator.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"
#include "ContentLengthHeaderValidator/ContentLengthHeaderValidator.hpp"
#include "TransferEncodingHeaderValidator/TransferEncodingHeaderValidator.hpp"

SimpleResult RequestValidator::validateRequestLineNew(const RequestLine &requestLine)
{
    if (requestLine.getMethod() == NOT_IMPLEMENTED)
        return SimpleResult::fail(NOT_IMPLEMENTED_ERR);

    if (requestLine.getTargetUri().length() > MAX_URI_LENGTH)
        return SimpleResult::fail(URI_TOO_LONG_ERR);

    if (requestLine.getHttpVersion() != "HTTP/1.1")
        return SimpleResult::fail(HTTP_VERSION_NOT_SUPPORTED_ERR);

    return SimpleResult::ok();
}

SimpleResult RequestValidator::validateRequestLine(const RequestLineParams_t &requestLine)
{
    RequestLine rql;

    rql.setMethod(requestLine.method);
    rql.setTargetUri(requestLine.target.uri);
    rql.setHttpVersion(requestLine.httpVersion);

    return validateRequestLineNew(rql);
}

SimpleResult RequestValidator::validateRequestHeaders(const HeaderCollection &requestHeaders)
{
    if (!requestHeaders.contains(HOST)
    || !HostHeaderValidator::isValid(requestHeaders.getHeader(HOST)))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    if (requestHeaders.contains(CONTENT_LENGTH)
        && requestHeaders.contains(TRANSFER_ENCODING))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    if (requestHeaders.contains(CONTENT_LENGTH)
        && !ContentLengthHeaderValidator::isValid(requestHeaders.getHeader(CONTENT_LENGTH)))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    if (requestHeaders.contains(TRANSFER_ENCODING)
        && !TransferEncodingHeaderValidator::isValid(requestHeaders.getHeader(TRANSFER_ENCODING)))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    if (requestHeaders.contains(CONNECTION)
        && !ConnectionHeaderValidator::isValid(requestHeaders.getHeader(CONNECTION)))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    return SimpleResult::ok();
}
