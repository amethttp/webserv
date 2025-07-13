#include "RequestTokenizer.hpp"

const std::string RequestTokenizer::tcharsSymbols = "!#$%&'*+-.^_`|~";
const std::string RequestTokenizer::unreservedSymbols = "-._~";
const std::string RequestTokenizer::subDelimSymbols = "!$&'()*+,;=";
const std::string RequestTokenizer::qdTextSymbols = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~ \t";

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

void RequestTokenizer::skipChunkExtensionAtDistance(int &distance) const
{
    int startingDistance = distance;
    char lastChunkExtensionChar = peek(startingDistance);

    if (lastChunkExtensionChar != ';')
        return;

    startingDistance++;
    lastChunkExtensionChar = peek(startingDistance);
    if (!isTchar(lastChunkExtensionChar))
        return;

    while (isTchar(lastChunkExtensionChar))
    {
        startingDistance++;
        lastChunkExtensionChar = peek(startingDistance);
    }

    if (lastChunkExtensionChar == '=')
    {
        startingDistance++;
        lastChunkExtensionChar = peek(startingDistance);

        if (isTchar(lastChunkExtensionChar))
        {
            while (isTchar(lastChunkExtensionChar))
            {
                startingDistance++;
                lastChunkExtensionChar = peek(startingDistance);
            }
        }
        else if (lastChunkExtensionChar == '\"')
        {
            startingDistance++;
            lastChunkExtensionChar = peek(startingDistance);

            while (isQdText(lastChunkExtensionChar) || isQuotedPairAtDistance(startingDistance))
            {
                if (isQuotedPairAtDistance(startingDistance))
                    startingDistance++;
                startingDistance++;
                lastChunkExtensionChar = peek(startingDistance);
            }
            startingDistance++;
        }
        else
            return;
    }

    distance = startingDistance;
}

bool RequestTokenizer::hasFinishedText() const
{
    return this->pos_ >= this->text_.length();
}

bool RequestTokenizer::isTchar() const
{
    return (std::isalnum(this->currentChar_) || tcharsSymbols.find(this->currentChar_) != std::string::npos);
}

bool RequestTokenizer::isTchar(const char c)
{
    return (std::isalnum(c) || tcharsSymbols.find(c) != std::string::npos);
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
    char headerChar = peek(distance);

    if (!isTchar(headerChar))
        return false;

    while (!hasFinishedText() && isTchar(headerChar))
    {
        distance++;
        headerChar = peek(distance);
    }

    return headerChar == ':';
}

bool RequestTokenizer::isFieldLine() const
{
    return (std::isprint(this->currentChar_) || this->currentChar_ == '\t');
}

bool RequestTokenizer::isLastChunk() const
{
    int distance = 0;
    char lastChunkChar = peek(distance);

    if (lastChunkChar != '0')
        return false;

    while (!hasFinishedText() && lastChunkChar == '0')
    {
        distance++;
        lastChunkChar = peek(distance);
    }

    skipChunkExtensionAtDistance(distance);

    return isCrlfAtDistance(distance);
}

bool RequestTokenizer::isQdText(const char c)
{
    return (std::isalnum(c) || qdTextSymbols.find(c) != std::string::npos);
}

bool RequestTokenizer::isQuotedPairAtDistance(const int distance) const
{
    const char currentChar = peek(distance);
    const char nextChar = peek(distance + 1);

    return (currentChar == '\\' && (std::isprint(nextChar) || nextChar == '\t'));
}

bool RequestTokenizer::isCrlf() const
{
    return this->currentChar_ == '\r' && peek() == '\n';
}

bool RequestTokenizer::isCrlfAtDistance(const int distance) const
{
    return peek(distance) == '\r' && peek(distance + 1) == '\n';
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

    while (!hasFinishedText() && isFieldLine())
    {
        headerString += this->currentChar_;
        advance();
    }

    return headerString;
}

std::string RequestTokenizer::lastChunk()
{
    std::string lastChunkString;

    while (!hasFinishedText() && std::isprint(this->currentChar_))
    {
        lastChunkString += this->currentChar_;
        advance();
    }

    lastChunkString += crlf();

    return lastChunkString;
}

std::string RequestTokenizer::crlf()
{
    advance(2);
    return "\r\n";
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

    if (isLastChunk())
        return RequestToken(LAST_CHUNK, lastChunk());

    if (isHeader())
        return RequestToken(HEADER, header());

    if (isTchar())
        return RequestToken(METHOD, method());

    if (isCrlf())
        return RequestToken(CRLF, crlf());

    return RequestToken(UNKNOWN, "UNKNOWN");
}
