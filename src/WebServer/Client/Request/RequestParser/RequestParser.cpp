#include "RequestParser.hpp"
#include "utils/string/string.hpp"

static method_t parseRequestMethod(const std::string &requestMethod)
{
    if (requestMethod == "GET")
        return GET;
    else if (requestMethod == "POST")
        return POST;
    else if (requestMethod == "DELETE")
        return DELETE;
    return NOT_IMPLEMENTED;
}

SimpleResult RequestParser::parseRequestLine(Request_t &request, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    if (splittedRequestLine.size() != 3)
        return SimpleResult::fail("400 Bad Request");

    if (splittedRequestLine[0].empty())
        return SimpleResult::fail("400 Bad Request");

    request.method = parseRequestMethod(splittedRequestLine[0]);

    if (request.method == NOT_IMPLEMENTED)
        return SimpleResult::fail("501 Not Implemented");

    request.target = splittedRequestLine[1];
    request.httpVersion = splittedRequestLine[2];

    return SimpleResult::ok();
}

void RequestParser::parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer)
{
    std::vector<std::string> header;
    std::vector<std::string>::iterator it = splittedRequestBuffer.begin() + 1;

    while (it != splittedRequestBuffer.end() && !it->empty())
    {
        header = split(*it, ":");
        request.headers[header[0]] = trim(header[1], " ");
        ++it;
    }
}

