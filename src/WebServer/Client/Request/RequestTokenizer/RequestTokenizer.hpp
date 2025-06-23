#pragma once

#include "RequestToken/RequestToken.hpp"
#include <string>

class RequestTokenizer
{
private:
    std::string text_;
    size_t pos_;
    char currentChar_;

    static const std::string tcharsSymbols;

    bool isTchar() const;
    bool isHttpVersion() const;

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    void advance();

    std::string token();
    std::string httpVersion();

    RequestToken getNextToken();
};
