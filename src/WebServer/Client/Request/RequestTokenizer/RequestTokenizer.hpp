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
    static const std::string qdTextSymbols;

    void advance(size_t amount = 1);
    char peek(size_t distance = 1) const;

    void skipTokenAtDistance(int &distance) const;
    void skipChunkExtensionsAtDistance(int &distance) const;

    bool hasFinishedText() const;
    bool isTchar() const;
    static bool isTchar(char c);
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
    bool isTokenAtDistance(int distance) const;
    bool isQuotedStringAtDistance(int distance) const;
    bool isChunkExtensionValAtDistance(int distance) const;
    bool isChunk() const;
    bool isChunkSize() const;
    bool isLastChunk() const;
    static bool isQdText(char c);
    bool isQuotedPairAtDistance(int distance) const;
    bool isCrlf() const;
    bool isCrlfAtDistance(int distance) const;

    std::string method();
    std::string httpVersion();
    std::string absolutePath();
    std::string query();
    std::string target();
    std::string sp();
    std::string header();
    std::string chunkSize();
    std::string crlf();

public:
    RequestTokenizer(const std::string &text);
    ~RequestTokenizer();

    RequestToken getNextToken();
    RequestToken getOctetStreamToken(size_t size);
};
