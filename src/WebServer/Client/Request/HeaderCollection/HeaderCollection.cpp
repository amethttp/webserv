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
    for (std::vector<Header>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); ++it)
    {
        if (it->getKey() == headerKey)
            return *it;
    }

    throw std::invalid_argument("Header not found inside the collection");
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
    const std::string headerKey = getHeaderKey(headerString);
    const std::string headerValue = trim(getHeaderValue(headerKey), " \t");

    if (contains(headerKey))
    {
        addToExistingHeader(headerKey, headerValue);
        return;
    }

    Header newHeader = Header(headerKey);
    newHeader.addValue(headerValue);
    this->headers_.push_back(newHeader);
}
