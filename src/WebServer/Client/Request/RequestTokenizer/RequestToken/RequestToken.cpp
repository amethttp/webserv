#include "RequestToken.hpp"

RequestToken::RequestToken(const std::string &type, const std::string &value)
{
    this->type_ = type;
    this->value_ = value;
}

RequestToken::~RequestToken() {}

std::string RequestToken::getType()
{
    return this->type_;
}

std::string RequestToken::getValue()
{
    return this->value_;
}

std::string RequestToken::toString() const
{
    return std::string("Token(") + this->type_ + "," + this->value_ + ")";
}


