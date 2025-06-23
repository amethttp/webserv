#include "RequestTokenizer.hpp"
#include <stdexcept>

const std::string RequestTokenizer::tcharsSymbols = "!#$%&'*+-.^_`|~";

RequestTokenizer::RequestTokenizer(const std::string &text)
{
    this->text_ = text;
    this->pos_ = 0;
    this->currentChar_ = text[this->pos_];
}

RequestTokenizer::~RequestTokenizer() {}

void RequestTokenizer::advance(const int amount)
{
    this->pos_ += amount;

    if (this->pos_ < this->text_.length())
        this->currentChar_ = this->text_[this->pos_];
}

bool RequestTokenizer::isTchar() const
{
    return (std::isalnum(this->currentChar_) || tcharsSymbols.find(this->currentChar_) != std::string::npos);
}

bool RequestTokenizer::isHttpVersion() const
{
    return (this->text_.find("HTTP/") == this->pos_
            && std::isdigit(this->text_[this->pos_ + 5])
            && this->text_[this->pos_ + 6] == '.'
            && std::isdigit(this->text_[this->pos_ + 7]));
}

std::string RequestTokenizer::token()
{
    std::string result;

    while (this->pos_ < this->text_.length() && isTchar())
    {
        result += this->text_[this->pos_];
        advance();
    }
    return result;
}

std::string RequestTokenizer::httpVersion()
{
    std::string result = this->text_.substr(this->pos_, 8);

    advance(8);

    return result;
}

RequestToken RequestTokenizer::getNextToken()
{
    if (this->pos_ >= this->text_.size())
        return RequestToken(EOF, "");

    this->currentChar_ = this->text_[this->pos_];

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

    if (isHttpVersion())
        return RequestToken(HTTP_VERSION, httpVersion());

    if (isTchar())
        return RequestToken(TOKEN, token());

    return RequestToken(UNKNOWN, "UNKNOWN");
}
