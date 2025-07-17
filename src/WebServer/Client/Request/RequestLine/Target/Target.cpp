#include "Target.hpp"
#include <algorithm>

void Target::separateUriComponents()
{
    const std::string::iterator uriBegin = this->uri_.begin();
    const std::string::iterator uriEnd = this->uri_.end();
    const std::string::iterator queryStart = std::find(uriBegin, uriEnd, '?');

    this->path_ = std::string(uriBegin, queryStart);
    if (queryStart != uriEnd)
        this->query_ = std::string(queryStart + 1, uriEnd);
}

Target::Target() {}

Target::~Target() {}

std::string Target::getUri() const
{
    return this->uri_;
}

std::string Target::getUriPath() const
{
    return this->path_;
}

std::string Target::getUriQuery() const
{
    return this->query_;
}

void Target::setUri(const std::string &uri)
{
    this->uri_ = uri;
    this->path_ = "";
    this->query_ = "";

    separateUriComponents();
}

void Target::setPath(const std::string &path)
{
    this->path_ = path;
}
