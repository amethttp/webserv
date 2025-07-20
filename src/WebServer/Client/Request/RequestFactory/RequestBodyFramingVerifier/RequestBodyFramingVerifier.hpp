#pragma once

#include <string>

class RequestBodyFramingVerifier
{
private:
    std::string text_;
    size_t pos_;
    char currentChar_;

    void advance(size_t amount = 1);
    char peek(size_t distance = 1) const;

    void skipChunkExtension();

    bool hasFinishedText() const;
    bool isLastChunk() const;
    bool isCrlf() const;

public:
    RequestBodyFramingVerifier(const std::string &bodyString);
    ~RequestBodyFramingVerifier();

    bool isFullBodyComplete(const size_t &contentLengthSize) const;
    bool isChunkedBodyComplete();
};
