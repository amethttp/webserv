#include "HeaderCollection.hpp"

HeaderCollection::HeaderCollection() {}

HeaderCollection::~HeaderCollection() {}

std::vector<Header> HeaderCollection::getHeaders() const
{
    return this->headers_;
}
