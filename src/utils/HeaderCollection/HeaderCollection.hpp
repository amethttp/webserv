#pragma once

#include <vector>
#include "Header/Header.hpp"

class HeaderCollection
{
private:
    std::vector<Header> headers_;

    void addToExistingHeader(const std::string &headerKey, const std::string &headerValue);

public:
    HeaderCollection();
    ~HeaderCollection();

    Header getHeader(const std::string &headerKey) const;
    size_t getAmountOfHeaders() const;

    bool contains(const std::string &headerKey) const;

    void addHeader(const std::string &headerString);
	void addHeader(const std::string &headerKey, const std::string &headerValue);
    void updateHeader(const std::string &headerKey, const std::string &newHeaderValue);
    void removeHeaders();

	friend std::ostream &operator<<(std::ostream &stream, HeaderCollection &headers);
};
