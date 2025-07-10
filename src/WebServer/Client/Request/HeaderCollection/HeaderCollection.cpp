#include "utils/HTTP/http.hpp"
#include "HeaderCollection.hpp"
#include "utils/string/string.hpp"
#include <stdexcept>

void HeaderCollection::addToExistingHeader(const std::string &headerKey, const std::string &headerValue)
{
    for (std::vector<Header>::iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            it->addValue(headerValue);
    }
}

HeaderCollection::HeaderCollection() {}

HeaderCollection::~HeaderCollection() {}

Header HeaderCollection::getHeader(const std::string &headerKey) const
{
    for (std::vector<Header>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            return *it;
    }

    throw std::invalid_argument("Header with key '" + headerKey + "' does not exist inside the collection");
}

std::vector<Header> HeaderCollection::getHeaders() const
{
    return this->headers_;
}

bool HeaderCollection::contains(const std::string &headerKey) const
{
    for (std::vector<Header>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            return true;
    }

    return false;
}

void HeaderCollection::addHeader(const std::string &headerString)
{
    const std::string headerKey = getHeaderKey(headerString);
    const std::string headerValue = trim(getHeaderValue(headerString), " \t");

    if (contains(headerKey))
    {
        addToExistingHeader(headerKey, headerValue);
        return;
    }

    Header newHeader = Header(headerKey);
    newHeader.addValue(headerValue);
    this->headers_.push_back(newHeader);
}

void HeaderCollection::addHeader(const Header &newHeader)
{
    this->headers_.push_back(newHeader);
}
