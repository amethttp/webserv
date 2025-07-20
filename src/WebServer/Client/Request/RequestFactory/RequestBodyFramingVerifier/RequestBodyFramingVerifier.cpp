#include "utils/numeric/numeric.hpp"
#include "RequestBodyFramingVerifier.hpp"

void RequestBodyFramingVerifier::advance(const size_t amount)
{
    this->pos_ += amount;

    if (!hasFinishedText())
        this->currentChar_ = this->text_[this->pos_];
    else
        this->currentChar_ = '\0';
}

char RequestBodyFramingVerifier::peek(const size_t distance) const
{
    const size_t peekedCharPos = this->pos_ + distance;

    if (peekedCharPos >= this->text_.length())
        return '\0';

    return this->text_[peekedCharPos];
}

bool RequestBodyFramingVerifier::hasFinishedText() const
{
    return this->pos_ >= this->text_.length();
}

bool RequestBodyFramingVerifier::isLastChunk() const
{
    int distance = 0;

    if (peek(distance) != '0')
        return false;

    while (peek(distance) == '0')
    {
        distance++;
    }

    return (peek(distance) == '\r' && peek(distance + 1) == '\n') || peek(distance) == ';';
}

bool RequestBodyFramingVerifier::isCrlf() const
{
    return this->currentChar_ == '\r' && peek() == '\n';
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
    while (!isLastChunk())
    {
        std::string chunkSize;

        if (!std::isxdigit(this->currentChar_))
            return false;

        while (!hasFinishedText() && std::isxdigit(this->currentChar_))
        {
            chunkSize += this->currentChar_;
            advance();
        }

        if (this->currentChar_ == ';')
        {
            advance();

            while (!hasFinishedText() && !isCrlf())
            {
                advance();
            }
        }

        if (!isCrlf())
            return false;

        advance(2);
        advance(hexToDec(chunkSize));

        while (!hasFinishedText() && !isCrlf())
        {
            advance();
        }

        advance(2);
    }

    if (this->currentChar_ != '0')
        return false;

    while (!hasFinishedText() && this->currentChar_ == '0')
    {
        advance();
    }

    if (this->currentChar_ == ';')
    {
        advance();

        while (!hasFinishedText() && !isCrlf())
        {
            advance();
        }

        advance(2);
    }

    return isCrlf();
}
