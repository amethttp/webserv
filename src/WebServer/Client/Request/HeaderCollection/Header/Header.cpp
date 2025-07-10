#include "Header.hpp"

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
