#include "RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"
#include <algorithm>

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

result_t RequestParser::eatTrailerFields()
{
    if (this->currentToken_.getType() != HEADER)
        return eat(CRLF);

    int hasFailed = 0;
    while (this->currentToken_.getType() == HEADER)
    {
        hasFailed |= eat(HEADER);
        hasFailed |= eat(CRLF);
    }

    return hasFailed ? FAIL : SUCCESS;
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

static bool isNotHex(const char c)
{
    return !std::isxdigit(c);
}

static size_t getChunkSize(const std::string &chunkSize)
{
    const std::string::const_iterator chunkSizeBegin = chunkSize.begin();
    const std::string::const_iterator chunkSizeEnd = chunkSize.end();
    const std::string::const_iterator chunkSizeValueEnd = std::find_if(chunkSizeBegin, chunkSizeEnd, isNotHex);
    const std::string chunkSizeValue = std::string(chunkSizeBegin, chunkSizeValueEnd);

    return hexToDec(chunkSizeValue);
}

Result<std::string> RequestParser::parseChunkedBody()
{
    int hasFailed = 0;
    std::string chunkedBody;

    while (this->currentToken_.getType() == CHUNK_SIZE)
    {
        const size_t chunkSize = getChunkSize(this->currentToken_.getValue());

        chunkedBody += eatOctetStreamToken(chunkSize);
        hasFailed |= eat(CRLF);
    }

    hasFailed |= eat(LAST_CHUNK);

    hasFailed |= eatTrailerFields();

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<std::string>::fail("400 Bad Request");

    return Result<std::string>::ok(chunkedBody);
}
