#include "utils/HTTP/http.hpp"
#include "HeaderCollection.hpp"
#include "utils/string/string.hpp"
#include <stdexcept>

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
