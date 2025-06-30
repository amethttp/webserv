#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
#include <algorithm>
#include <vector>

void RequestFactory::splitRequestTargetComponents(Target_t &target)
{
    const std::string::iterator uriBegin = target.uri.begin();
    const std::string::iterator uriEnd = target.uri.end();
    const std::string::iterator queryStart = std::find(uriBegin, uriEnd, '?');

    target.path = std::string(uriBegin, queryStart);
    if (queryStart != uriEnd)
        target.query = std::string(queryStart + 1, uriEnd);
}

Result<std::string> RequestFactory::decodePath(const std::string &path)
{
    std::string decodedPath;

    for (size_t i = 0; i < path.length(); i++)
    {
        if (path[i] != '%')
        {
            decodedPath += path[i];
            continue ;
        }

        const char decodedChar = hexToChar(path[i + 1], path[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<std::string>::fail("400 Bad Request");

        decodedPath += decodedChar;
        i += 2;
    }

    return Result<std::string>::ok(decodedPath);
}

bool RequestFactory::isEncodedQueryValid(const std::string &query)
{
    for (size_t i = 0; i < query.length(); i++)
    {
        if (query[i] != '%')
            continue;

        const char decodedChar = hexToChar(query[i + 1], query[i + 2]);

        if (std::iscntrl(decodedChar))
            return false;
    }

    return true;
}

Result<Target_t> RequestFactory::decodeTarget(const Target_t &encodedTarget)
{
    Target_t decodedTarget;

    const Result<std::string> decodingPathResult = decodePath(encodedTarget.path);
    if (decodingPathResult.isFailure())
        return Result<Target_t>::fail(decodingPathResult.getError());

    if (!isEncodedQueryValid(encodedTarget.query))
        return Result<Target_t>::fail("400 Bad Request");

    decodedTarget.uri = encodedTarget.uri;
    decodedTarget.path = decodingPathResult.getValue();
    decodedTarget.query = encodedTarget.query;

    return Result<Target_t>::ok(decodedTarget);
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
    const RequestTokenizer tokenizer = RequestTokenizer(requestLineString);
    RequestParser requestParser = RequestParser(tokenizer);

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



    splitRequestTargetComponents(request.requestLine.target);

    Result<Target_t> decodingTargetResult = decodeTarget(request.requestLine.target);

    if (decodingTargetResult.isFailure())
        return Result<Request_t>::fail(decodingTargetResult.getError());

    request.requestLine.target = decodingTargetResult.getValue();



    RequestParser requestParser = RequestParser(RequestTokenizer(requestLine));

    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}
