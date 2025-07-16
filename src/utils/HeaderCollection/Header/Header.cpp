#include "Header.hpp"
#include "utils/string/string.hpp"

std::string Header::toHeaderCase(const std::string &string)
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

Header::Header(const std::string &key)
{
    this->key_ = toHeaderCase(key);
}

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
    const std::string trimmedValue = trim(newValue, " \t");

    this->values_.push_back(trimmedValue);
}

void Header::removeValues()
{
    this->values_.clear();
}

std::string Header::getHeaderKey(const std::string &headerString)
{
    const size_t headerKeyEnd = headerString.find(':');
    const std::string headerKey = headerString.substr(0, headerKeyEnd);

    return headerKey;
}

std::string Header::getHeaderValue(const std::string &headerString)
{
    const size_t headerValueStart = headerString.find(':') + 1;
    const std::string headerValue = headerString.substr(headerValueStart);

    return headerValue;
}

std::ostream &operator<<(std::ostream &stream, const Header &header)
{
	stream << header.getKey() << static_cast<std::string>(": ") << header.getValue();

	return stream;
}
