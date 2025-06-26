#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"

RequestParser::RequestParser(const RequestTokenizer &tokenizer)
    : tokenizer_(tokenizer), currentToken_(tokenizer_.getNextToken())
{
}

method_t RequestParser::parseRequestMethod(const std::string &requestMethod)
{
    if (requestMethod == "GET")
        return GET;
    if (requestMethod == "POST")
        return POST;
    if (requestMethod == "DELETE")
        return DELETE;
    return NOT_IMPLEMENTED;
}

result_t RequestParser::eat(tokenType_t type)
{
    if (this->currentToken_.getType() != type)
        return FAIL;

    this->currentToken_ = this->tokenizer_.getNextToken();
    return SUCCESS;
}

Result<RequestLineParams_t> RequestParser::parseRequestLine()
{
    int hasFailed = 0;
    RequestLineParams_t params;

    params.method = parseRequestMethod(this->currentToken_.getValue());
    hasFailed |= eat(TOKEN);

    hasFailed |= eat(SP);

    params.target = this->currentToken_.getValue();
    hasFailed |= eat(TARGET);

    hasFailed |= eat(SP);

    params.httpVersion = this->currentToken_.getValue();
    hasFailed |= eat(HTTP_VERSION);

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<RequestLineParams_t>::fail("400 Bad Request");

    if (params.method == NOT_IMPLEMENTED)
        return Result<RequestLineParams_t>::fail("501 Not Implemented");

    if (params.target.length() > 8000)
        return Result<RequestLineParams_t>::fail("414 URI Too Long");

    if (params.httpVersion != "HTTP/1.1")
        return Result<RequestLineParams_t>::fail("505 HTTP Version Not Supported");

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

