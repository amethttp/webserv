#include "RequestValidator.hpp"
#include "utils/string/string.hpp"
#include "utils/headers/headers.hpp"
#include "utils/numeric/numeric.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"
#include <stdlib.h>

bool RequestValidator::isUnreserved(const char c)
{
    const std::string unreservedSymbols = "-._~";

    return (std::isalnum(c) || unreservedSymbols.find(c) != std::string::npos);
}

bool RequestValidator::isPctEncoded(const std::string &header, const size_t pos)
{
    if (pos + 2 >= header.length())
        return false;

    return (header[pos] == '%'
            && std::isxdigit(header[pos + 1])
            && std::isxdigit(header[pos + 2]));
}

bool RequestValidator::isSubDelim(const char c)
{
    const std::string subDelimSymbols = "!$&'()*+,;=";

    return subDelimSymbols.find(c) != std::string::npos;
}

bool RequestValidator::isRegName(const std::string &header, const size_t pos)
{
    return (isUnreserved(header[pos]) || isPctEncoded(header, pos) || isSubDelim(header[pos]));
}

bool RequestValidator::isValidHostName(const std::string &hostName)
{
    if (hostName.empty())
        return false;

    for (size_t i = 0; i < hostName.length(); i++)
    {
        if (!isRegName(hostName, i))
            return false;
    }

    return true;
}

bool RequestValidator::isValidHostPort(const std::string &port)
{
    if (port.empty() || !isLong(port))
        return false;

    const long portNum = std::atol(port.c_str());

    return (portNum >= 0 && portNum <= 65535);
}

bool RequestValidator::isValidHostHeader(const headerValue_t &hostHeaderValues)
{
    if (hostHeaderValues.size() != 1)
        return false;

    const std::string hostValue = hostHeaderValues.front();
    const size_t portSeparator = hostValue.find(':');
    const std::string hostName = hostValue.substr(0, portSeparator);
    std::string hostPort;

    if (portSeparator != std::string::npos)
        hostPort = hostValue.substr(portSeparator + 1);

    if (!isValidHostName(hostName))
        return false;
    if (portSeparator != std::string::npos)
        return isValidHostPort(hostPort);

    return true;
}

bool RequestValidator::isValidContentLengthHeader(const headerValue_t &contentLengthHeaderValues)
{
    if (contentLengthHeaderValues.size() != 1)
        return false;

    const std::string contentLengthValue = contentLengthHeaderValues.front();
    if (!isLong(contentLengthValue))
        return false;

    const long contentLengthSize = std::atol(contentLengthValue.c_str());
    return contentLengthSize >= 0;
}

bool RequestValidator::isValidTransferEncodingHeader(const headerValue_t &transferEncodingHeaderValues)
{
    if (transferEncodingHeaderValues.size() != 1)
        return false;

    const std::string transferEncodingValue = toLower(transferEncodingHeaderValues.front());

    return transferEncodingValue == "chunked";
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

    if (containsHeader(requestHeaders, "Content-Length")
        && !isValidContentLengthHeader(requestHeaders.at("Content-Length")))
        return SimpleResult::fail("400 Bad Request");

    if (containsHeader(requestHeaders, "Transfer-Encoding")
        && !isValidTransferEncodingHeader(requestHeaders.at("Transfer-Encoding")))
        return SimpleResult::fail("400 Bad Request");

    return SimpleResult::ok();
}
