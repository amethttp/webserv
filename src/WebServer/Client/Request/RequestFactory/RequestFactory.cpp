#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include <vector>

static void parseRequestLine(RequestInfo_t &requestInfo, const std::string &requestLine)
{
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    if (splittedRequestLine[0] == "GET")
        requestInfo.request.method = GET;
    else if (splittedRequestLine[0] == "POST")
        requestInfo.request.method = POST;
    else
        requestInfo.request.method = DELETE;

    requestInfo.request.target = splittedRequestLine[1];
    requestInfo.request.httpVersion = splittedRequestLine[2];
}

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    parseRequestLine(requestInfo, splittedRequestBuffer[0]);

    std::vector<std::string> header;
    std::vector<std::string>::iterator it = splittedRequestBuffer.begin() + 1;
    while (it != splittedRequestBuffer.end() && !it->empty())
    {
        header = split(*it, ": ");
        requestInfo.request.headers[header[0]] = header[1];
        ++it;
    }

    return Result<RequestInfo_t>::ok(requestInfo);
}