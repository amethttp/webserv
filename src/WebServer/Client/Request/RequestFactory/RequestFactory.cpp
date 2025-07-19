#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "utils/numeric/numeric.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "RequestValidator/RequestValidator.hpp"
#include "RequestProcesser/RequestProcesser.hpp"
#include "RequestBodyFramingVerifier/RequestBodyFramingVerifier.hpp"

std::string RequestFactory::getRequestLineString(const std::string &requestString)
{
    const size_t requestLineEnd = requestString.find("\r\n");

    return requestString.substr(0, requestLineEnd);
}

std::string RequestFactory::getRequestHeadersString(const std::string &requestString)
{
    const size_t requestLineEnd = requestString.find("\r\n");
    const size_t headersEnd = requestString.find("\r\n\r\n");
    const size_t headersSize = headersEnd - requestLineEnd - 2;

    return requestString.substr(requestLineEnd + 2, headersSize);
}

std::string RequestFactory::getRequestBodyString(const std::string &requestString)
{
    const size_t headersEnd = requestString.find("\r\n\r\n");

    return requestString.substr(headersEnd + 4);
}

RequestParser RequestFactory::createParser(const std::string &text)
{
    const RequestTokenizer tokenizer(text);
    return RequestParser(tokenizer);
}

Result<RequestLine> RequestFactory::buildRequestLineFromString(const std::string &requestLineString)
{
    RequestParser requestParser = createParser(requestLineString);

    const Result<RequestLine> requestLineResult = requestParser.parseRequestLine();
    if (requestLineResult.isFailure())
        return Result<RequestLine>::fail(requestLineResult.getError());
    RequestLine requestLine = requestLineResult.getValue();

    const SimpleResult requestLineValidationResult = RequestValidator::validateRequestLine(requestLine);
    if (requestLineValidationResult.isFailure())
        return Result<RequestLine>::fail(requestLineValidationResult.getError());

    const SimpleResult requestTargetProcessResult = RequestProcesser::processRequestLine(requestLine);
    if (requestTargetProcessResult.isFailure())
        return Result<RequestLine>::fail(requestTargetProcessResult.getError());

    return Result<RequestLine>::ok(requestLine);
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

Result<Body> RequestFactory::buildFullBodyFromString(const size_t &contentLengthSize, const std::string &bodyString)
{
    RequestParser requestParser = createParser(bodyString);

    const Result<Body> requestBodyResult = requestParser.parseFullBody(contentLengthSize);
    if (requestBodyResult.isFailure())
        return Result<Body>::fail(requestBodyResult.getError());

    return Result<Body>::ok(requestBodyResult.getValue());
}

Result<Body> RequestFactory::buildChunkedBodyFromString(const std::string &bodyString)
{
    RequestParser requestParser = createParser(bodyString);

    const Result<Body> requestBodyResult = requestParser.parseChunkedBody();
    if (requestBodyResult.isFailure())
        return Result<Body>::fail(requestBodyResult.getError());

    return Result<Body>::ok(requestBodyResult.getValue());
}

Result<Body> RequestFactory::buildRequestBodyFromString(const HeaderCollection &headers, const std::string &bodyString)
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
        return Result<Body>::fail(LENGTH_REQUIRED_ERR);
    }

    return Result<Body>::ok(Body());
}

Result<Request_t> RequestFactory::create(const std::string &requestString)
{
    Request_t request;
    const std::string requestLineString = getRequestLineString(requestString);
    const std::string requestHeadersString = getRequestHeadersString(requestString);
    const std::string requestBodyString = getRequestBodyString(requestString);

    const Result<RequestLine> requestLineResult = buildRequestLineFromString(requestLineString);
    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());
    request.requestLine = requestLineResult.getValue();

    const Result<HeaderCollection> requestHeadersResult = buildRequestHeadersFromString(requestHeadersString);
    if (requestHeadersResult.isFailure())
        return Result<Request_t>::fail(requestHeadersResult.getError());
    request.headers = requestHeadersResult.getValue();

    const Result<Body> requestBodyResult = buildRequestBodyFromString(request.headers, requestBodyString);
    if (requestBodyResult.isFailure())
        return Result<Request_t>::fail(requestBodyResult.getError());
    request.body = requestBodyResult.getValue();

    return Result<Request_t>::ok(request);
}

bool RequestFactory::canCreateAResponse(const std::string &requestString)
{
    if (requestString.find("\r\n\r\n") == std::string::npos)
        return false;

    const std::string headersString = getRequestHeadersString(requestString);
    const Result<HeaderCollection> headersResult = buildRequestHeadersFromString(headersString);
    if (headersResult.isFailure())
        return true;

    const HeaderCollection headers = headersResult.getValue();

    if (headers.contains("Content-Length"))
    {
        const std::string bodyString = getRequestBodyString(requestString);
        const size_t contentLengthSize = strToUlong(headers.getHeaderValue(CONTENT_LENGTH));
        const RequestBodyFramingVerifier requestBodyFramingVerifier = RequestBodyFramingVerifier(bodyString);

        return requestBodyFramingVerifier.isFullBodyComplete(contentLengthSize);
    }

    if (headers.contains("Transfer-Encoding"))
    {
        const std::string bodyString = getRequestBodyString(requestString);
        RequestBodyFramingVerifier requestBodyFramingVerifier = RequestBodyFramingVerifier(bodyString);

        return requestBodyFramingVerifier.isChunkedBodyComplete();
    }

    return true;
}
