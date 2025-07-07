#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "RequestValidator/RequestValidator.hpp"
#include "RequestTargetProcesser/RequestTargetProcesser.hpp"
#include <vector>

RequestParser RequestFactory::createParser(const std::string &text)
{
    const RequestTokenizer tokenizer(text);
    return RequestParser(tokenizer);
}

Result<RequestLineParams_t> RequestFactory::buildRequestLineFromString(const std::string &requestLineString)
{
    RequestParser requestParser = createParser(requestLineString);

    const Result<RequestLineParams_t> requestLineResult = requestParser.parseRequestLine();
    if (requestLineResult.isFailure())
        return Result<RequestLineParams_t>::fail(requestLineResult.getError());
    RequestLineParams_t requestLineParams = requestLineResult.getValue();

    const SimpleResult requestLineValidationResult = RequestValidator::validateRequestLine(requestLineParams);
    if (requestLineValidationResult.isFailure())
        return Result<RequestLineParams_t>::fail(requestLineValidationResult.getError());

    const SimpleResult requestTargetProcessResult = RequestTargetProcesser::process(requestLineParams.target);
    if (requestTargetProcessResult.isFailure())
        return Result<RequestLineParams_t>::fail(requestTargetProcessResult.getError());

    return Result<RequestLineParams_t>::ok(requestLineParams);
}

Result<Request_t> RequestFactory::create(const std::string &requestBuffer)
{
    Request_t request;

    const size_t requestLineEnd = requestBuffer.find("\r\n");
    const size_t headersEnd = requestBuffer.find("\r\n\r\n");
    const size_t headersSize = headersEnd - requestLineEnd - 2;
    const std::string requestLineString = requestBuffer.substr(0, requestLineEnd);
    const std::string headersString = requestBuffer.substr(requestLineEnd + 2, headersSize);



    const Result<RequestLineParams_t> requestLineResult = buildRequestLineFromString(requestLineString);
    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());
    request.requestLine = requestLineResult.getValue();



    RequestParser requestParser = createParser(headersString);

    const Result<headers_t> requestHeadersResult = requestParser.parseHeaders();
    if (requestHeadersResult.isFailure())
        return Result<Request_t>::fail(requestHeadersResult.getError());
    request.headers = requestHeadersResult.getValue();

    return Result<Request_t>::ok(request);
}
