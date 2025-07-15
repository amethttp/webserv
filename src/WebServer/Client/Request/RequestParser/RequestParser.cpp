#include "RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
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

std::string RequestParser::eatOctetStreamToken(const size_t &streamSize)
{
    std::string octetStream = this->tokenizer_.getOctetStreamToken(streamSize).getValue();

    this->currentToken_ = this->tokenizer_.getNextToken();

    return octetStream;
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

Result<HeaderCollection> RequestParser::parseHeaders()
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

Result<std::string> RequestParser::parseChunkedBody()
{
    int hasFailed = 0;
    std::string chunkedBody;

    while (this->currentToken_.getType() == CHUNK_SIZE)
    {
        const std::string chunkSizeString = this->currentToken_.getValue();
        const std::string chunkSizeValue = chunkSizeString.substr(0, chunkSizeString.find("\r\n"));
        const size_t chunkSize = hexToDec(chunkSizeValue.substr(0, chunkSizeString.find(';')));

        chunkedBody += eatOctetStreamToken(chunkSize);
        hasFailed |= eat(CRLF);
    }

    hasFailed |= eat(LAST_CHUNK);

    if (this->currentToken_.getType() != HEADER)
        hasFailed |= eat(CRLF);

    while (this->currentToken_.getType() == HEADER)
    {
        hasFailed |= eat(HEADER);
        hasFailed |= eat(CRLF);
    }

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<std::string>::fail("400 Bad Request");

    return Result<std::string>::ok(chunkedBody);
}
