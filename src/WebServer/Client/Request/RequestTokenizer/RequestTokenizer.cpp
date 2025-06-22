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

void RequestTokenizer::advance()
{
    this->pos_++;

    if (this->pos_ < this->text_.length())
        this->currentChar_ = this->text_[this->pos_];
}

bool RequestTokenizer::isTchar() const
{
    const char ch = this->currentChar_;

    return (std::isalnum(ch) || tcharsSymbols.find(ch) != std::string::npos);
}

bool RequestTokenizer::isHttpVersion() const
{
    return (this->text_.find("HTTP/") == this->pos_
            && std::isdigit(this->text_[this->pos_ + 5])
            && this->text_[this->pos_ + 6] == '.'
            && std::isdigit(this->text_[this->pos_ + 7]));
}

std::string RequestTokenizer::httpMethod()
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
    std::string result;

    while (this->pos_ < this->text_.length() && std::isprint(this->currentChar_) && this->currentChar_ != ' ')
    {
        if (std::isalpha(this->currentChar_) && this->currentChar_ >= 'a')
            return result;
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

    if (isTchar() && this->pos_ == 0)
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

    if (isHttpVersion())
        return RequestToken(HTTP_VERSION, httpVersion());

    return RequestToken(EOF, "");
}
