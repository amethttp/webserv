#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include <vector>

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;
    RequestParser requestParser;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    SimpleResult requestLineResult = requestParser.parseRequestLine(requestInfo.request, splittedRequestBuffer[0]);

    if (requestLineResult.isFailure())
        return Result<RequestInfo_t>::fail(requestLineResult.getError());

    requestParser.parseHeaders(requestInfo.request, splittedRequestBuffer);

    return Result<RequestInfo_t>::ok(requestInfo);
}