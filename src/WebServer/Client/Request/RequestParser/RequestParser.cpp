#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include <vector>

static Result<method_t> parseRequestMethod(const std::string &requestMethod)
{
    if (requestMethod == "GET")
        return Result<method_t>::ok(GET);
    else if (requestMethod == "POST")
        return Result<method_t>::ok(POST);
    else if (requestMethod == "DELETE")
        return Result<method_t>::ok(DELETE);
    else if (!requestMethod.empty())
        return Result<method_t>::fail("501 Not Implemented");
    else
        return Result<method_t>::fail("400 Bad Request");
}

Result<bool> RequestParser::parseRequestLine(Request_t &request, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    if (splittedRequestLine.size() != 3)
        return Result<bool>::fail("400 Bad Request");

    Result<method_t> methodResult = parseRequestMethod(splittedRequestLine[0]);
    
    if (methodResult.isFailure())
        return Result<bool>::fail(methodResult.getError());
        
    request.method = methodResult.getValue();
    request.target = splittedRequestLine[1];
    request.httpVersion = splittedRequestLine[2];

    return Result<bool>::ok(true);
}
