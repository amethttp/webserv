#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestToken/RequestToken.hpp"
#include <vector>

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
    void error();

public:
    RequestParser(const RequestTokenizer &tokenizer);

    Result<RequestLineParams_t> parseRequestLine();
    void parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer);
};
