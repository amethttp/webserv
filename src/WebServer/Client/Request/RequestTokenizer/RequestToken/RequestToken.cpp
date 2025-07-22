#include "RequestToken.hpp"

RequestToken::RequestToken(const t_tokenType type, const std::string &value)
{
    this->type_ = type;
    this->value_ = value;
}

RequestToken::~RequestToken() {}

t_tokenType RequestToken::getType() const
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


