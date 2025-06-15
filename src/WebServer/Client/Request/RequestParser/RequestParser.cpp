#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include <vector>

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

Result<bool> RequestParser::parseRequestLine(Request_t &request, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    if (splittedRequestLine.size() != 3)
        return Result<bool>::fail("400 Bad Request");

    if (splittedRequestLine[0].empty())
        return Result<bool>::fail("400 Bad Request");

    request.method = parseRequestMethod(splittedRequestLine[0]);
    
    if (request.method == NOT_IMPLEMENTED)
        return Result<bool>::fail("501 Not Implemented");

    request.target = splittedRequestLine[1];
    request.httpVersion = splittedRequestLine[2];

    return Result<bool>::ok(true);
}
