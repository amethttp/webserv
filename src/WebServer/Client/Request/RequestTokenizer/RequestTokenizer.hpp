#pragma once

#include "RequestToken/RequestToken.hpp"
#include <string>

class RequestTokenizer
{
private:
    std::string text_;
    size_t pos_;
    char currentChar_;

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    void advance();

    std::string httpMethod();

    RequestToken getNextToken();
    void error() const;
};
