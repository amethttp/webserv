#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "utils/numeric/numeric.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "RequestValidator/RequestValidator.hpp"
#include "RequestProcesser/RequestProcesser.hpp"

std::string RequestFactory::getRequestLineString(const std::string &requestBuffer)
{
    const size_t requestLineEnd = requestBuffer.find("\r\n");

    return requestBuffer.substr(0, requestLineEnd);
}

std::string RequestFactory::getRequestHeadersString(const std::string &requestBuffer)
{
    const size_t requestLineEnd = requestBuffer.find("\r\n");
    const size_t headersEnd = requestBuffer.find("\r\n\r\n");
    const size_t headersSize = headersEnd - requestLineEnd - 2;

    return requestBuffer.substr(requestLineEnd + 2, headersSize);
}

std::string RequestFactory::getRequestBodyString(const std::string &requestBuffer)
{
    const size_t headersEnd = requestBuffer.find("\r\n\r\n");

    return requestBuffer.substr(headersEnd + 4);
}

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

    const SimpleResult requestTargetProcessResult = RequestProcesser::processRequestTarget(requestLineParams.target);
    if (requestTargetProcessResult.isFailure())
        return Result<RequestLineParams_t>::fail(requestTargetProcessResult.getError());

    return Result<RequestLineParams_t>::ok(requestLineParams);
}

Result<HeaderCollection> RequestFactory::buildRequestHeadersFromString(const std::string &headersString)
{
    RequestParser requestParser = createParser(headersString);

    const Result<HeaderCollection> requestHeadersResult = requestParser.parseHeaders();
    if (requestHeadersResult.isFailure())
        return Result<HeaderCollection>::fail(requestHeadersResult.getError());
    HeaderCollection requestHeaders = requestHeadersResult.getValue();

    const SimpleResult requestHeaderValidationResult = RequestValidator::validateRequestHeaders(requestHeaders);
    if (requestHeaderValidationResult.isFailure())
        return Result<HeaderCollection>::fail(requestHeaderValidationResult.getError());

    const SimpleResult headerProcessingResult = RequestProcesser::processHeaders(requestHeaders);
    if (headerProcessingResult.isFailure())
        return Result<HeaderCollection>::fail(headerProcessingResult.getError());

    return Result<HeaderCollection>::ok(requestHeaders);
}

Result<std::string> RequestFactory::buildFullBodyFromString(const size_t &contentLengthSize, const std::string &bodyString)
{
    RequestParser requestParser = createParser(bodyString);

    const Result<std::string> requestBodyResult = requestParser.parseFullBody(contentLengthSize);
    if (requestBodyResult.isFailure())
        return Result<std::string>::fail(requestBodyResult.getError());

    return Result<std::string>::ok(requestBodyResult.getValue());
}

Result<std::string> RequestFactory::buildChunkedBodyFromString(const std::string &bodyString)
{
    RequestParser requestParser = createParser(bodyString);

    const Result<std::string> requestBodyResult = requestParser.parseChunkedBody();
    if (requestBodyResult.isFailure())
        return Result<std::string>::fail(requestBodyResult.getError());

    return Result<std::string>::ok(requestBodyResult.getValue());
}

Result<std::string> RequestFactory::buildRequestBodyFromString(const HeaderCollection &headers, const std::string &bodyString)
{
    if (headers.contains(CONTENT_LENGTH))
    {
        const size_t contentLengthSize = strToUlong(headers.getHeaderValue(CONTENT_LENGTH));
        return buildFullBodyFromString(contentLengthSize, bodyString);
    }

    if (headers.contains(TRANSFER_ENCODING))
    {
        return buildChunkedBodyFromString(bodyString);
    }

    if (!bodyString.empty())
    {
        return Result<std::string>::fail(LENGTH_REQUIRED_ERR);
    }

    return Result<std::string>::ok(bodyString);
}

Result<Request_t> RequestFactory::create(const std::string &requestBuffer)
{
    Request_t request;
    const std::string requestLineString = getRequestLineString(requestBuffer);
    const std::string requestHeadersString = getRequestHeadersString(requestBuffer);
    const std::string requestBodyString = getRequestBodyString(requestBuffer);

    const Result<RequestLineParams_t> requestLineResult = buildRequestLineFromString(requestLineString);
    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());
    request.requestLine = requestLineResult.getValue();

    const Result<HeaderCollection> requestHeadersResult = buildRequestHeadersFromString(requestHeadersString);
    if (requestHeadersResult.isFailure())
        return Result<Request_t>::fail(requestHeadersResult.getError());
    request.headers = requestHeadersResult.getValue();

    const Result<std::string> requestBodyResult = buildRequestBodyFromString(request.headers, requestBodyString);
    if (requestBodyResult.isFailure())
        return Result<Request_t>::fail(requestBodyResult.getError());
    request.body = requestBodyResult.getValue();

    return Result<Request_t>::ok(request);
}
