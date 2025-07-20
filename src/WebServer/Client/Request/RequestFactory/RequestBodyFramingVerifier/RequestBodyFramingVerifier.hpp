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
    void skipUntilNextCrlf();
    void skipTrailerFields();

    bool consumeChunkSizeIfComplete();
    void consumeChunkData(size_t chunkSize);

    bool hasFinishedText() const;
    bool isLastChunk() const;
    bool isCrlf() const;
    bool isCrlfAtDistance(size_t distance) const;
    bool isChunkExtensionAtDistance(size_t distance) const;

    std::string getChunkSize() const;

public:
    RequestBodyFramingVerifier(const std::string &bodyString);
    ~RequestBodyFramingVerifier();

    bool isFullBodyComplete(const size_t &contentLengthSize) const;
    bool isChunkedBodyComplete();
};
