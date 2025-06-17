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

void RequestParser::eat(tokenType_t type)
{
    if (this->currentToken_.getType() == type)
        this->currentToken_ = this->tokenizer_.getNextToken();
    else
        error();
}

void RequestParser::error()
{
    throw std::invalid_argument("Request Parser Error");
}

Result<RequestLineParams_t> RequestParser::parseRequestLine()
{
    RequestLineParams_t params;

    try
    {
        params.method = parseRequestMethod(this->currentToken_.getValue());
        eat(METHOD);

        eat(SP);

        params.target = this->currentToken_.getValue();
        eat(TARGET);

        eat(SP);

        params.httpVersion = this->currentToken_.getValue();
        eat(HTTP_VERSION);

        if (params.method == NOT_IMPLEMENTED)
            return Result<RequestLineParams_t>::fail("501 Not Implemented");
    }
    catch (const std::invalid_argument &e)
    {
        return Result<RequestLineParams_t>::fail("400 Bad Request");
    }

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

