#include "headers.hpp"
#include "utils/HTTP/http.hpp"
#include "utils/string/string.hpp"

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
