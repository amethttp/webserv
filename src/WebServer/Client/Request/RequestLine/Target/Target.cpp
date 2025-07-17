#include "Target.hpp"

Target::Target() {}

Target::~Target() {}

std::string Target::getUri() const
{
    return this->uri_;
}

void Target::setUri(const std::string &uri)
{
    this->uri_ = uri;
}
