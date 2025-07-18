#include "RequestParser.hpp"
#include "utils/numeric/numeric.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"

RequestParser::RequestParser(const RequestTokenizer &tokenizer)
    : tokenizer_(tokenizer), currentToken_(EMPTY, "")
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

Result<RequestLine> RequestParser::parseRequestLine()
{
    int hasFailed = 0;
    RequestLine requestLine;

    hasFailed |= eat(EMPTY);

    requestLine.setMethod(this->currentToken_.getValue());
    hasFailed |= eat(METHOD);

    hasFailed |= eat(SP);

    requestLine.setTargetUri(this->currentToken_.getValue());
    hasFailed |= eat(TARGET);

    hasFailed |= eat(SP);

    requestLine.setHttpVersion(this->currentToken_.getValue());
    hasFailed |= eat(HTTP_VERSION);

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<RequestLine>::fail(BAD_REQUEST_ERR);

    return Result<RequestLine>::ok(requestLine);
}

Result<HeaderCollection> RequestParser::parseHeaders()
{
    int hasFailed = 0;
    HeaderCollection headers;

    hasFailed |= eat(EMPTY);

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
        return Result<HeaderCollection>::fail(BAD_REQUEST_ERR);

    return Result<HeaderCollection>::ok(headers);
}

Result<Body> RequestParser::parseFullBody(const size_t contentLengthSize)
{
    Body body;
    const std::string bodyMessage = eatOctetStreamToken(contentLengthSize);

    if (eat(EOF) == FAIL)
        return Result<Body>::fail(BAD_REQUEST_ERR);

    body.addFragment(bodyMessage);

    return Result<Body>::ok(body);
}

Result<Body> RequestParser::parseChunkedBody()
{
    int hasFailed = 0;
    Body body;

    hasFailed |= eat(EMPTY);

    while (this->currentToken_.getType() == CHUNK_SIZE)
    {
        const size_t chunkSize = Body::getChunkSize(this->currentToken_.getValue());
        const std::string chunkData = eatOctetStreamToken(chunkSize);

        body.addFragment(chunkData);

        hasFailed |= eat(CRLF);
    }

    hasFailed |= eat(LAST_CHUNK);

    hasFailed |= eatTrailerFields();

    hasFailed |= eat(EOF);

    if (hasFailed)
        return Result<Body>::fail(BAD_REQUEST_ERR);

    return Result<Body>::ok(body);
}

bool RequestParser::isCompleteChunkedBody()
{
    do
    {
        this->currentToken_ = this->tokenizer_.getOctetStreamToken(1);
    }
    while (this->currentToken_.getValue() == "0");

    std::string crlf = this->currentToken_.getValue();
    this->currentToken_ = this->tokenizer_.getOctetStreamToken(1);
    crlf += this->currentToken_.getValue();

    if (crlf != "\r\n")
        return false;

    return true;
}
