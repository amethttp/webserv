#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"

RequestParser::RequestParser(const RequestTokenizer &tokenizer)
    : tokenizer_(tokenizer), currentToken_(tokenizer_.getNextToken())
{
}

static method_t parseRequestMethod(const std::string &requestMethod)
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
    hasFailed |= eat(METHOD);

    hasFailed |= eat(SP);

    params.target = this->currentToken_.getValue();
    hasFailed |= eat(TARGET);

    hasFailed |= eat(SP);

    params.httpVersion = this->currentToken_.getValue();
    hasFailed |= eat(HTTP_VERSION);

    if (hasFailed)
        return Result<RequestLineParams_t>::fail("400 Bad Request");

    if (params.method == NOT_IMPLEMENTED)
        return Result<RequestLineParams_t>::fail("501 Not Implemented");

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

