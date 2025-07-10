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

    Header getHeader(const std::string &headerKey) const;
    std::vector<Header> getHeaders() const;

    bool contains(const std::string &headerKey) const;

    void addHeader(const std::string &headerString);
    void addHeader(const Header &newHeader);
};
