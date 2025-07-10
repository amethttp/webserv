#pragma once

#include "Header/Header.hpp"
#include <vector>

class HeaderCollection
{
private:
    std::vector<Header> headers_;

    void addToExistingHeader(const std::string &headerKey, const std::string &headerValue);

public:
    HeaderCollection();
    ~HeaderCollection();

    std::vector<Header> getHeaders() const;

    bool contains(const std::string &headerKey) const;

    void addHeader(const std::string &headerString);
};
