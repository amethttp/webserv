#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "RequestTargetDecoder/RequestTargetDecoder.hpp"
#include "RequestTargetProcesser/RequestTargetProcesser.hpp"
#include <algorithm>
#include <vector>

RequestParser RequestFactory::createParser(const std::string &text)
{
    const RequestTokenizer tokenizer(text);
    return RequestParser(tokenizer);
}

Result<RequestLineParams_t> RequestFactory::validateRequestLine(const RequestLineParams_t &requestLine)
{
    if (requestLine.method == NOT_IMPLEMENTED)
        return Result<RequestLineParams_t>::fail("501 Not Implemented");

    if (requestLine.target.uri.length() > MAX_URI_LENGTH)
        return Result<RequestLineParams_t>::fail("414 URI Too Long");

    if (requestLine.httpVersion != "HTTP/1.1")
        return Result<RequestLineParams_t>::fail("505 HTTP Version Not Supported");

    return Result<RequestLineParams_t>::ok(requestLine);
}

Result<RequestLineParams_t> RequestFactory::getRequestLine(const std::string &requestLineString)
{
    RequestParser requestParser = createParser(requestLineString);

    const Result<RequestLineParams_t> requestLineResult = requestParser.parseRequestLine();

    if (requestLineResult.isFailure())
        return Result<RequestLineParams_t>::fail(requestLineResult.getError());

    return validateRequestLine(requestLineResult.getValue());
}

Result<Request_t> RequestFactory::create(const std::string &requestBuffer)
{
    Request_t request;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");
    const std::string requestLine = splittedRequestBuffer[0];



    const Result<RequestLineParams_t> requestLineResult = getRequestLine(requestLine);
    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());
    request.requestLine = requestLineResult.getValue();



    const SimpleResult requestTargetResult = RequestTargetProcesser::process(request.requestLine.target);
    if (requestTargetResult.isFailure())
        return Result<Request_t>::fail(requestTargetResult.getError());



    RequestParser requestParser = createParser(requestLine);
    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}
