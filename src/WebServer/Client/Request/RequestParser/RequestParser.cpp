#include "RequestParser.hpp"
#include "utils/string/string.hpp"
#include "utils/headers/headers.hpp"
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

Result<HeaderCollection> RequestParser::parseHeadersNew()
{
    int hasFailed = 0;
    HeaderCollection headers;

    do
    {
        headers.addHeader(this->currentToken_.getValue());
        hasFailed |= eat(HEADER);

        if (this->currentToken_.getType() != EOF)
            hasFailed |= eat(CRLF);
    }
    while (this->currentToken_.getType() == HEADER);

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<HeaderCollection>::fail("400 Bad Request");

    return Result<HeaderCollection>::ok(headers);
}

Result<headers_t> RequestParser::parseHeaders()
{
    const Result<HeaderCollection> headersResult = parseHeadersNew();
    if (headersResult.isFailure())
        return Result<headers_t>::fail(headersResult.getError());

    return Result<headers_t>::ok(collectionToHeaders(headersResult.getValue()));
}
