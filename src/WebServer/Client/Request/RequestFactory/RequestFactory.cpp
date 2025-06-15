#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include <vector>

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;
    RequestParser requestParser;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    Result<RequestLineParams_t> requestLineResult = requestParser.parseRequestLine(splittedRequestBuffer[0]);

    if (requestLineResult.isFailure())
        return Result<RequestInfo_t>::fail(requestLineResult.getError());

    requestInfo.request.requestLine = requestLineResult.getValue();

    requestParser.parseHeaders(requestInfo.request, splittedRequestBuffer);

    return Result<RequestInfo_t>::ok(requestInfo);
}