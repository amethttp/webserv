#include "headers.hpp"
#include "utils/HTTP/http.hpp"
#include "utils/string/string.hpp"

bool tryAddHeader(headers_t &headers, const std::string &header)
{
    const std::string headerKey = getHeaderKey(header);
    const std::string headerValue = getHeaderValue(header);

    if (headerKey.empty())
        return false;

    headers[headerKey] = trim(headerValue, " \t");
    return true;
}
