#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestToken/RequestToken.hpp"

#define MAX_URI_LENGTH 8000

typedef enum result_e
{
    SUCCESS,
    FAIL
} result_t;

class RequestParser
{
private:
    RequestTokenizer tokenizer_;
    RequestToken currentToken_;

    result_t eat(tokenType_t type);

    std::string eatOctetStreamToken(const size_t &streamSize);
    result_t eatTrailerFields();

public:
    RequestParser(const RequestTokenizer &tokenizer);

    Result<RequestLineParams_t> parseRequestLine();
    Result<HeaderCollection> parseHeaders();
    Result<std::string> parseFullBody(const size_t contentLengthSize);
    Result<std::string> parseChunkedBody();
};
