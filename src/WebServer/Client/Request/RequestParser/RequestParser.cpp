#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"

RequestParser::RequestParser(const RequestTokenizer &tokenizer)
    : tokenizer_(tokenizer), currentToken_(tokenizer_.getNextToken())
{
}

result_t RequestParser::eat(const tokenType_t type)
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

    params.method = getHttpMethodFromString(this->currentToken_.getValue());
    hasFailed |= eat(METHOD);

    hasFailed |= eat(SP);

    params.target.uri = this->currentToken_.getValue();
    hasFailed |= eat(TARGET);

    hasFailed |= eat(SP);

    params.httpVersion = this->currentToken_.getValue();
    hasFailed |= eat(HTTP_VERSION);

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<RequestLineParams_t>::fail("400 Bad Request");

    return Result<RequestLineParams_t>::ok(params);
}

Result<headers_t> RequestParser::parseHeaders()
{
    headers_t headers;

    const std::string header = this->currentToken_.getValue();
    if (eat(HEADER) == FAIL || eat(EOF) == FAIL)
        return Result<headers_t>::fail("400 Bad Request");

    const std::string headerKey = toHttpHeaderCase(header.substr(0, header.find(':')));
    const std::string headerValue = header.substr(header.find(':') + 1);

    if (headerKey.empty())
        return Result<headers_t>::fail("400 Bad Request");

    headers[headerKey] = trim(headerValue, " \t");

    return Result<headers_t>::ok(headers);
}

