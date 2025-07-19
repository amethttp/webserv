#include "RequestBodyFramingVerifier.hpp"

RequestBodyFramingVerifier::RequestBodyFramingVerifier(const std::string &bodyString)
{
    this->text_ = bodyString;
    this->pos_ = 0;
}

RequestBodyFramingVerifier::~RequestBodyFramingVerifier() {}

bool RequestBodyFramingVerifier::isFullBodyComplete(const size_t &contentLengthSize) const
{
    return this->text_.length() >= contentLengthSize;
}

bool RequestBodyFramingVerifier::isChunkedBodyComplete()
{
    if (this->text_[this->pos_] != '0')
        return false;

    while (this->pos_ < this->text_.length() && this->text_[this->pos_] == '0')
    {
        this->pos_++;
    }

    if (this->text_[this->pos_] != '\r')
        return false;

    return true;
}
