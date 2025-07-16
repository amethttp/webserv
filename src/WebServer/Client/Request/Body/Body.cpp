#include "Body.hpp"

Body::Body() {}

Body::~Body() {}

std::string Body::getMessage()
{
    return this->message_;
}

void Body::addFragment(const std::string &fragment)
{
    this->message_ += fragment;
}
