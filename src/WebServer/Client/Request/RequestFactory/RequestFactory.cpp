#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include <vector>

static method_t parseRequestMethod(const std::string &requestMethod)
{
    if (requestMethod == "GET")
        return GET;
    else if (requestMethod == "POST")
        return POST;
    else
        return DELETE;
}

static void parseRequestLine(Request_t &request, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    request.method = parseRequestMethod(splittedRequestLine[0]);
    request.target = splittedRequestLine[1];
    request.httpVersion = splittedRequestLine[2];
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

    parseRequestLine(requestInfo.request, splittedRequestBuffer[0]);
    parseHeaders(requestInfo.request, splittedRequestBuffer);

    return Result<RequestInfo_t>::ok(requestInfo);
}