#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
#include <vector>

void RequestFactory::splitRequestTargetComponents(Target_t &target)
{
    target.path = "/";
    target.query = "";
}

Result<std::string> RequestFactory::decodeTarget(const std::string &encodedTarget)
{
    std::string decodedTarget;
    const size_t queryStartPos = encodedTarget.find('?') > encodedTarget.length() ? encodedTarget.length() : encodedTarget.find('?');
    const std::string absolutePath = encodedTarget.substr(0, queryStartPos);
    const std::string query = encodedTarget.substr(queryStartPos);

    for (size_t i = 0; i < absolutePath.length(); i++)
    {
        if (encodedTarget[i] != '%')
        {
            decodedTarget += encodedTarget[i];
            continue ;
        }

        const char decodedChar = hexToChar(encodedTarget[i + 1], encodedTarget[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<std::string>::fail("400 Bad Request");

        decodedTarget += decodedChar;
        i += 2;
    }

    for (size_t i = 0; i < query.length(); i++)
    {
        if (query[i] != '%')
        {
            decodedTarget += query[i];
            continue;
        }

        const char decodedChar = hexToChar(query[i + 1], query[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<std::string>::fail("400 Bad Request");

        decodedTarget += query[i];
    }

    return Result<std::string>::ok(decodedTarget);
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

    Result<std::string> decodingTargetResult = decodeTarget(request.requestLine.target.uri);

    if (decodingTargetResult.isFailure())
        return Result<Request_t>::fail(decodingTargetResult.getError());

    request.requestLine.target.uri = decodingTargetResult.getValue();

    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}
