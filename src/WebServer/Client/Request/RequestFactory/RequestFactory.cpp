#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include <vector>

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

    Result<bool> requestLineResult = RequestParser::parseRequestLine(requestInfo.request, splittedRequestBuffer[0]);

    if (requestLineResult.isFailure())
        return Result<RequestInfo_t>::fail(requestLineResult.getError());

    parseHeaders(requestInfo.request, splittedRequestBuffer);

    return Result<RequestInfo_t>::ok(requestInfo);
}