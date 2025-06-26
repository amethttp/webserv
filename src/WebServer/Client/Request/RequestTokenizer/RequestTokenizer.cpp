#include "RequestTokenizer.hpp"
#include "utils/numeric/numeric.hpp"

const std::string RequestTokenizer::tcharsSymbols = "!#$%&'*+-.^_`|~";
const std::string RequestTokenizer::unreservedSymbols = "-._~";
const std::string RequestTokenizer::subDelimSymbols = "!$&'()*+,;=";

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

    if (!hasFinishedText())
        this->currentChar_ = this->text_[this->pos_];
}

char RequestTokenizer::peek(const size_t distance) const
{
    const size_t peekedCharacterPos = this->pos_ + distance;

    if (peekedCharacterPos >= this->text_.length())
        return '\0';

    return this->text_[peekedCharacterPos];
}

bool RequestTokenizer::hasFinishedText() const
{
    return this->pos_ >= this->text_.length();
}

bool RequestTokenizer::isTchar() const
{
    return (std::isalnum(this->currentChar_) || tcharsSymbols.find(this->currentChar_) != std::string::npos);
}

bool RequestTokenizer::startsWithHttpPrefixAtCurrentPos() const
{
    return this->text_.find("HTTP/") == this->pos_;
}

bool RequestTokenizer::isHttpVersion() const
{
    return (startsWithHttpPrefixAtCurrentPos()
            && std::isdigit(peek(5))
            && peek(6) == '.'
            && std::isdigit(peek(7)));
}

bool RequestTokenizer::isUnreserved() const
{
    return (std::isalnum(this->currentChar_) || unreservedSymbols.find(this->currentChar_) != std::string::npos);
}

bool RequestTokenizer::isPctEncoded() const
{
    return (this->currentChar_ == '%'
            && isHexdig(peek())
            && isHexdig(peek(2)));
}

bool RequestTokenizer::isSubDelim() const
{
    return subDelimSymbols.find(this->currentChar_) != std::string::npos;
}

bool RequestTokenizer::isPcharSymbol() const
{
    return (this->currentChar_ == ':' || this->currentChar_ == '@');
}

bool RequestTokenizer::isPchar() const
{
    return (isUnreserved() || isPctEncoded() || isSubDelim() || isPcharSymbol());
}

std::string RequestTokenizer::token()
{
    std::string tokenString;

    while (!hasFinishedText() && isTchar())
    {
        tokenString += this->currentChar_;
        advance();
    }

    return tokenString;
}

std::string RequestTokenizer::httpVersion()
{
    const int httpVersionLength = 8;
    std::string httpVersionString = this->text_.substr(this->pos_, httpVersionLength);

    advance(httpVersionLength);

    return httpVersionString;
}

bool RequestTokenizer::isAbsolutePath() const
{
    return (this->currentChar_ == '/' || isPchar());
}

std::string RequestTokenizer::target()
{
    std::string targetString = "/";

    advance();
    while (!hasFinishedText() && isAbsolutePath())
    {
        targetString += this->currentChar_;
        advance();
    }

    if (this->currentChar_ != '?')
        return targetString;

    targetString += this->currentChar_;
    advance();

    return targetString;
}

RequestToken RequestTokenizer::getNextToken()
{
    if (hasFinishedText())
        return RequestToken(EOF, "");

    if (this->currentChar_ == ' ')
    {
        advance();
        return RequestToken(SP, " ");
    }

    if (this->currentChar_ == '/')
        return RequestToken(TARGET, target());

    if (isHttpVersion())
        return RequestToken(HTTP_VERSION, httpVersion());

    if (isTchar())
        return RequestToken(TOKEN, token());

    return RequestToken(UNKNOWN, "UNKNOWN");
}
