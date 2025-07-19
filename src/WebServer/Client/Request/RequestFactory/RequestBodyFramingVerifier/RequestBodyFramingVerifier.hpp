#pragma once

#include <string>

class RequestBodyFramingVerifier
{
private:
    std::string text;
    size_t pos;

public:
    RequestBodyFramingVerifier(const std::string &bodyString);
    ~RequestBodyFramingVerifier();

    bool isFullBodyComplete(const size_t &contentLengthSize) const;
    bool isChunkedBodyComplete();
};
