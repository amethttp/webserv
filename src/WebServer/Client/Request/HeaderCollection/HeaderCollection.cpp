#include "HeaderCollection.hpp"
#include "utils/string/string.hpp"
#include <stdexcept>

void HeaderCollection::addToExistingHeader(const std::string &headerKey, const std::string &headerValue)
{
    for (std::vector<Header>::iterator it = this->headers_.begin(); it != this->headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            return it->addValue(headerValue);
    }
}

HeaderCollection::HeaderCollection() {}

HeaderCollection::~HeaderCollection() {}

Header HeaderCollection::getHeader(const std::string &headerKey) const
{
    for (std::vector<Header>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            return *it;
    }

    throw std::invalid_argument("Header with key '" + headerKey + "' does not exist inside the collection");
}

std::string HeaderCollection::getHeaderValue(const std::string &headerKey) const
{
    return this->getHeader(headerKey).getValue();
}

size_t HeaderCollection::getAmountOfHeaders() const
{
    return this->headers_.size();
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
    const std::string headerKey = Header::getHeaderKey(headerString);
    const std::string headerValue = Header::getHeaderValue(headerString);

    if (this->contains(headerKey))
        return this->addToExistingHeader(headerKey, headerValue);

    Header newHeader = Header(headerKey);
    newHeader.addValue(headerValue);
    this->headers_.push_back(newHeader);
}

void HeaderCollection::updateHeader(const std::string &headerKey, const std::string &newHeaderValue)
{
    for (std::vector<Header>::iterator it = this->headers_.begin(); it != this->headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
        {
            it->removeValues();
            it->addValue(newHeaderValue);
            return;
        }
    }
}

void HeaderCollection::removeHeaders()
{
    this->headers_.clear();
}
