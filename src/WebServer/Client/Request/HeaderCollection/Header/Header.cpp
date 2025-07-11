#include "Header.hpp"
#include "utils/string/string.hpp"

std::string Header::toHttpHeaderCase(const std::string &string)
{
    std::string result = string;

    result[0] = static_cast<char>(std::toupper(result[0]));
    for (size_t i = 1; i < result.length(); i++)
    {
        if (result[i - 1] == '-')
            result[i] = static_cast<char>(std::toupper(result[i]));
        else
            result[i] = static_cast<char>(std::tolower(result[i]));
    }

    return result;
}

std::string Header::getHeaderKey(const std::string &header)
{
    const size_t headerKeyEnd = header.find(':');
    const std::string headerKey = header.substr(0, headerKeyEnd);

    return toHttpHeaderCase(headerKey);
}

std::string Header::getHeaderValue(const std::string &header)
{
    const size_t headerValueStart = header.find(':') + 1;
    const std::string headerValue = header.substr(headerValueStart);

    return trim(headerValue, " \t");
}

Header::Header(const std::string &key) : key_(key) {}

Header::~Header() {}

const std::string &Header::getKey() const
{
    return this->key_;
}

const std::string &Header::getValue() const
{
    return this->values_.back();
}

size_t Header::getAmountOfValues() const
{
    return this->values_.size();
}

void Header::addValue(const std::string &newValue)
{
    this->values_.push_back(newValue);
}

void Header::removeValues()
{
    this->values_.clear();
}
