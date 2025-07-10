#pragma once

#include "Header/Header.hpp"
#include <vector>


class HeaderCollection
{
private:
    std::vector<Header> headers_;

public:
    HeaderCollection();
    ~HeaderCollection();

    Header getHeader(const std::string &headerKey) const;
    size_t getAmountOfHeaders() const;
};
