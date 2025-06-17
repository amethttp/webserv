#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"

static method_t parseRequestMethod(const std::string &requestMethod)
{
    RequestTokenizer tokenizer(requestMethod);
    RequestToken token = tokenizer.getNextToken();

    if (token.getValue() == "GET")
        return GET;
    if (token.getValue() == "POST")
        return POST;
    if (token.getValue() == "DELETE")
        return DELETE;
    return NOT_IMPLEMENTED;
}

Result<RequestLineParams_t> RequestParser::parseRequestLine(const std::string &requestLine)
{
    RequestLineParams_t params;
    std::vector<std::string> splittedRequestLine = split(requestLine, " ");

    if (splittedRequestLine.size() != 3)
        return Result<RequestLineParams_t>::fail("400 Bad Request");

    if (splittedRequestLine[0].empty())
        return Result<RequestLineParams_t>::fail("400 Bad Request");

    params.method = parseRequestMethod(splittedRequestLine[0]);

    if (params.method == NOT_IMPLEMENTED)
        return Result<RequestLineParams_t>::fail("501 Not Implemented");

    params.target = splittedRequestLine[1];
    params.httpVersion = splittedRequestLine[2];

    return Result<RequestLineParams_t>::ok(params);
}

void RequestParser::parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer)
{
    std::vector<std::string> header;
    std::vector<std::string>::iterator it = splittedRequestBuffer.begin() + 1;

    while (it != splittedRequestBuffer.end() && !it->empty())
    {
        header = split(*it, ":");
        request.headers[header[0]] = trim(header[1], " ");
        ++it;
    }
}

