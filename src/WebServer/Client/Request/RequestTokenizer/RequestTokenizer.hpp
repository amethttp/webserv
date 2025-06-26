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
    static const std::string unreservedSymbols;
    static const std::string subDelimSymbols;

    void advance(int amount = 1);
    char peek(size_t distance = 1) const;

    bool hasFinishedText() const;
    bool isTchar() const;
    bool startsWithHttpPrefixAtCurrentPos() const;
    bool isHttpVersion() const;
    bool isUnreserved() const;
    bool isPctEncoded() const;
    bool isSubDelim() const;
    bool isPcharSymbol() const;
    bool isPchar() const;
    bool isAbsolutePath() const;

    std::string token();
    std::string httpVersion();
    std::string target();

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    RequestToken getNextToken();
};
