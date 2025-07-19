#pragma once

#include <string>

class RequestBodyFramingVerifier
{
private:
    std::string text_;
    size_t pos_;

public:
    RequestBodyFramingVerifier(const std::string &bodyString);
    ~RequestBodyFramingVerifier();

    bool isFullBodyComplete(const size_t &contentLengthSize) const;
    bool isChunkedBodyComplete();
};
