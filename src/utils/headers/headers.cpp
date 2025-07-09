#include "headers.hpp"
#include "utils/HTTP/http.hpp"
#include "utils/string/string.hpp"
#include <stdexcept>

void addHeader(headers_t &headers, const std::string &header)
{
    const std::string headerKey = getHeaderKey(header);
    const std::string headerValue = trim(getHeaderValue(header), " \t");

    headers[headerKey].push_back(headerValue);
}

bool containsHeader(const headers_t &headers, const std::string &headerKey)
{
    return headers.find(headerKey) != headers.end();
}

std::string getHeader(const headers_t &headers, const std::string &headerKey)
{
    if (!containsHeader(headers, headerKey))
        throw std::logic_error("The header '" + headerKey + "' does not exist");

    return headers.at(headerKey).back();
}

void updateHeader(headers_t &headers, const std::string &headerKey, const std::string &newValue)
{
    if (!containsHeader(headers, headerKey))
        throw std::logic_error("The header '" + headerKey + "' does not exist");

    headers[headerKey].clear();
    headers[headerKey].push_back(newValue);
}
