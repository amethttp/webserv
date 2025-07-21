#include "RequestToken.hpp"

RequestToken::RequestToken(const tokenType_t type, const std::string &value)
{
    this->type_ = type;
    this->value_ = value;
}

RequestToken::~RequestToken() {}

tokenType_t RequestToken::getType() const
{
    return this->type_;
}

std::string RequestToken::getValue()
{
    return this->value_;
}

std::string RequestToken::toString() const
{
    return std::string("Token(") + this->value_ + ")";
}


