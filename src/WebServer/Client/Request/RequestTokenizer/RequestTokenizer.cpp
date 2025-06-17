#include "RequestTokenizer.hpp"
#include <stdexcept>

RequestTokenizer::RequestTokenizer(const std::string &text)
{
    this->text_ = text;
    this->pos_ = 0;
    this->currentChar_ = text[this->pos_];
}

RequestTokenizer::~RequestTokenizer() {}

void RequestTokenizer::advance()
{
    this->pos_++;

    if (this->pos_ < this->text_.length())
        this->currentChar_ = this->text_[this->pos_];
}

std::string RequestTokenizer::httpMethod()
{
    std::string result;

    while (this->pos_ < this->text_.length() && std::isalpha(this->currentChar_))
    {
        result += this->text_[this->pos_];
        advance();
    }
    return result;
}

std::string RequestTokenizer::httpVersion()
{
    std::string result;

    while (this->pos_ < this->text_.length() && std::isprint(this->currentChar_))
    {
        result += this->text_[this->pos_];
        advance();
    }
    return result;
}

RequestToken RequestTokenizer::getNextToken()
{
    if (this->pos_ >= this->text_.size())
        return RequestToken(EOF, "");

    this->currentChar_ = this->text_[this->pos_];

    if (std::isalpha(this->currentChar_) && this->pos_ == 0)
        return RequestToken(METHOD, httpMethod());

    if (this->currentChar_ == ' ')
    {
        advance();
        return RequestToken(SP, " ");
    }

    if (this->currentChar_ == '/')
    {
        advance();
        return RequestToken(TARGET, "/");
    }

    if (std::isalpha(this->currentChar_))
        return RequestToken(HTTP_VERSION, httpVersion());

    return RequestToken(EOF, "");
}

void RequestTokenizer::error() const
{
    const std::string message = std::string("Invalid character: ") + this->currentChar_;

    throw std::invalid_argument(message);
}


