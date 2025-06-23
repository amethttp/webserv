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

    void advance(int amount = 1);

    bool isTchar() const;
    bool startsWithHttpPrefixAtCurrentPos() const;
    char peekCharAtDistanceFromCurrentPos(size_t distanceFromCurrentPos) const;
    bool isHttpVersion() const;

    std::string token();
    std::string httpVersion();

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    RequestToken getNextToken();
};
