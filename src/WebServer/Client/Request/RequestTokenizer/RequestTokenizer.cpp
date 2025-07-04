#include "RequestTokenizer.hpp"

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
            && std::isxdigit(peek())
            && std::isxdigit(peek(2)));
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

bool RequestTokenizer::isAbsolutePath() const
{
    return (this->currentChar_ == '/' || isPchar());
}

bool RequestTokenizer::isQuery() const
{
    return (isPchar() || this->currentChar_ == '/' || this->currentChar_ == '?');
}

bool RequestTokenizer::isHeader() const
{
    int distance = 0;

    while (!hasFinishedText())
    {
        const char c = peek(distance);
        if (!(std::isalnum(c) || tcharsSymbols.find(c) != std::string::npos))
            break ;
        distance++;
    }

    if (peek(distance) != ':')
        return false;

    return true;
}

std::string RequestTokenizer::method()
{
    std::string methodString;

    while (!hasFinishedText() && isTchar())
    {
        methodString += this->currentChar_;
        advance();
    }

    return methodString;
}

std::string RequestTokenizer::httpVersion()
{
    const int httpVersionLength = 8;
    std::string httpVersionString = this->text_.substr(this->pos_, httpVersionLength);

    advance(httpVersionLength);

    return httpVersionString;
}

std::string RequestTokenizer::absolutePath()
{
    std::string absolutePathString;

    while (!hasFinishedText() && isAbsolutePath())
    {
        absolutePathString += this->currentChar_;
        advance();
    }

    return absolutePathString;
}

std::string RequestTokenizer::query()
{
    std::string queryString;

    while (!hasFinishedText() && isQuery())
    {
        queryString += this->currentChar_;
        advance();
    }

    return queryString;
}

std::string RequestTokenizer::target()
{
    std::string targetString = absolutePath();

    if (this->currentChar_ == '?')
        targetString += query();

    return targetString;
}

std::string RequestTokenizer::sp()
{
    advance();
    return " ";
}

std::string RequestTokenizer::header()
{
    std::string headerString;

    while (!hasFinishedText() && std::isprint(this->currentChar_))
    {
        headerString += this->currentChar_;
        advance();
    }

    return headerString;
}

RequestToken RequestTokenizer::getNextToken()
{
    if (hasFinishedText())
        return RequestToken(EOF, "");

    if (this->currentChar_ == ' ')
        return RequestToken(SP, sp());

    if (this->currentChar_ == '/')
        return RequestToken(TARGET, target());

    if (isHttpVersion())
        return RequestToken(HTTP_VERSION, httpVersion());

    if (isHeader())
        return RequestToken(HEADER, header());

    if (isTchar())
        return RequestToken(METHOD, method());

    return RequestToken(UNKNOWN, "UNKNOWN");
}
