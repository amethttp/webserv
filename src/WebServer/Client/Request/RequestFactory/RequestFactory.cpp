#include "RequestFactory.hpp"
#include "utils/string/string.hpp"
#include "../RequestParser/RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
#include <vector>

std::string RequestFactory::decodeTarget(const std::string &encodedTarget)
{
    std::string decodedTarget;

    for (size_t i = 0; i < encodedTarget.length(); i++)
    {
        if (encodedTarget[i] != '%')
        {
            decodedTarget += encodedTarget[i];
            continue ;
        }

        const char decodedChar = hexToChar(encodedTarget[i + 1], encodedTarget[i + 2]);

        if (std::iscntrl(decodedChar))
            return "";

        decodedTarget += decodedChar;
        i += 2;
    }

    return decodedTarget;
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

    if (request.requestLine.target.length() > MAX_URI_LENGTH)
        return Result<Request_t>::fail("414 URI Too Long");

    if (request.requestLine.httpVersion != "HTTP/1.1")
        return Result<Request_t>::fail("505 HTTP Version Not Supported");

    request.requestLine.target = decodeTarget(request.requestLine.target);

    if (request.requestLine.target.empty())
        return Result<Request_t>::fail("400 Bad Request");

    requestParser.parseHeaders(request, splittedRequestBuffer);

    return Result<Request_t>::ok(request);
}
