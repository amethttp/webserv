#include "RequestBodyFramingVerifier.hpp"

void RequestBodyFramingVerifier::advance()
{
    this->pos_++;

    if (!hasFinishedText())
        this->currentChar_ = this->text_[this->pos_];
    else
        this->currentChar_ = '\0';
}

bool RequestBodyFramingVerifier::hasFinishedText() const
{
    return this->pos_ >= this->text_.length();
}

RequestBodyFramingVerifier::RequestBodyFramingVerifier(const std::string &bodyString)
{
    this->text_ = bodyString;
    this->pos_ = 0;
    this->currentChar_ = this->text_[this->pos_];
}

RequestBodyFramingVerifier::~RequestBodyFramingVerifier() {}

bool RequestBodyFramingVerifier::isFullBodyComplete(const size_t &contentLengthSize) const
{
    return this->text_.length() >= contentLengthSize;
}

bool RequestBodyFramingVerifier::isChunkedBodyComplete()
{
    if (this->currentChar_ != '0')
        return false;

    while (!hasFinishedText() && this->currentChar_ == '0')
    {
        advance();
    }

    if (this->currentChar_ == ';')
    {
        advance();

        while (!hasFinishedText() && this->currentChar_ != '\r')
        {
            advance();
        }
    }

    if (this->currentChar_ != '\r')
        return false;

    return true;
}
