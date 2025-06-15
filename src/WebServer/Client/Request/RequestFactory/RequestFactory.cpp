#include "RequestFactory.hpp"
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

static Result<bool> parseRequestLine(Request_t &request, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    Result<method_t> methodResult = parseRequestMethod(splittedRequestLine[0]);
    
    if (methodResult.isFailure())
        return Result<bool>::fail(methodResult.getError());
        
    request.method = methodResult.getValue();
    request.target = splittedRequestLine[1];
    request.httpVersion = splittedRequestLine[2];

    return Result<bool>::ok(true);
}

static void parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer)
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

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    Result<bool> requestLineResult = parseRequestLine(requestInfo.request, splittedRequestBuffer[0]);

    if (requestLineResult.isFailure())
        return Result<RequestInfo_t>::fail(requestLineResult.getError());

    parseHeaders(requestInfo.request, splittedRequestBuffer);

    return Result<RequestInfo_t>::ok(requestInfo);
}