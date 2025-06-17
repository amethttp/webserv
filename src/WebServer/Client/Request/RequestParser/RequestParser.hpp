#pragma once

#include "../Request.hpp"
#include "utils/Result/Result.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestTokenizer.hpp"
#include "WebServer/Client/Request/RequestTokenizer/RequestToken/RequestToken.hpp"
#include <vector>

class RequestParser
{
private:
    RequestTokenizer tokenizer_;
    RequestToken currentToken_;

    void eat(tokenType_t type);
    void error();

public:
    RequestParser(const RequestTokenizer &tokenizer);

    Result<RequestLineParams_t> parseRequestLine(const std::string &requestLine);
    void parseHeaders(Request_t &request, std::vector<std::string> &splittedRequestBuffer);
};
