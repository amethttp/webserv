#include "RequestFactory.hpp"

#include <algorithm>

#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
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

Result<Target_t> RequestFactory::decodeTarget(const Target_t &encodedTarget)
{
    Target_t decodedTarget;
    const std::string path = encodedTarget.path;
    const std::string query = encodedTarget.query;

    decodedTarget.uri = encodedTarget.uri;
    for (size_t i = 0; i < path.length(); i++)
    {
        if (path[i] != '%')
        {
            decodedTarget.path += path[i];
            continue ;
        }

        const char decodedChar = hexToChar(path[i + 1], path[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<Target_t>::fail("400 Bad Request");

        decodedTarget.path += decodedChar;
        i += 2;
    }

    for (size_t i = 0; i < query.length(); i++)
    {
        if (query[i] != '%')
        {
            decodedTarget.query += query[i];
            continue;
        }

        const char decodedChar = hexToChar(query[i + 1], query[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<Target_t>::fail("400 Bad Request");

        decodedTarget.query += query[i];
    }

    return Result<Target_t>::ok(decodedTarget);
}

Result<Request_t> RequestFactory::create(const std::string &requestBuffer)
{
    Request_t request;

    std::vector<std::string> splittedRequestBuffer = split(requestBuffer, "\r\n");

    std::string requestLine = splittedRequestBuffer[0];
    RequestTokenizer tokenizer = RequestTokenizer(requestLine);
    RequestParser requestParser = RequestParser(tokenizer);



    Result<RequestLineParams_t> requestLineResult = requestParser.parseRequestLine();

    if (requestLineResult.isFailure())
        return Result<Request_t>::fail(requestLineResult.getError());

    request.requestLine = requestLineResult.getValue();

    if (request.requestLine.method == NOT_IMPLEMENTED)
        return Result<Request_t>::fail("501 Not Implemented");

    if (request.requestLine.target.uri.length() > MAX_URI_LENGTH)
        return Result<Request_t>::fail("414 URI Too Long");

    if (request.requestLine.httpVersion != "HTTP/1.1")
        return Result<Request_t>::fail("505 HTTP Version Not Supported");



    splitRequestTargetComponents(request.requestLine.target);

    Result<Target_t> decodingTargetResult = decodeTarget(request.requestLine.target);

    if (decodingTargetResult.isFailure())
        return Result<Request_t>::fail(decodingTargetResult.getError());

    request.requestLine.target = decodingTargetResult.getValue();

    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}
