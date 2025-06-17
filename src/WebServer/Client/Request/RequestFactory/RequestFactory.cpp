#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include <vector>

Result<Request_t> RequestFactory::create(const std::string &requestBuffer)
{
    Request_t request;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    RequestTokenizer tokenizer = RequestTokenizer(splittedRequestBuffer[0]);
    RequestParser requestParser = RequestParser(tokenizer);

    Result<RequestLineParams_t> requestLineResult = requestParser.parseRequestLine(splittedRequestBuffer[0]);

    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());

    request.requestLine = requestLineResult.getValue();

    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}