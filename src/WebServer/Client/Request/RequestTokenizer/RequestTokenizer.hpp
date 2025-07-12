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
    bool isTchar(char c) const;
    bool startsWithHttpPrefixAtCurrentPos() const;
    bool isHttpVersion() const;
    bool isUnreserved() const;
    bool isPctEncoded() const;
    bool isSubDelim() const;
    bool isPcharSymbol() const;
    bool isPchar() const;
    bool isAbsolutePath() const;
    bool isQuery() const;
    bool isHeader() const;
    bool isFieldLine() const;
    bool isLastChunk() const;
    bool isCrlf() const;
    bool isCrlfAtDistance(int distance) const;

    std::string method();
    std::string httpVersion();
    std::string absolutePath();
    std::string query();
    std::string target();
    std::string sp();
    std::string header();
    std::string lastChunk();
    std::string crlf();

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    RequestToken getNextToken();
};
