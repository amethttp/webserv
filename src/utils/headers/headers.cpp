#include "headers.hpp"
#include "utils/HTTP/http.hpp"
#include "utils/string/string.hpp"

void addHeader(headers_t &headers, const std::string &header)
{
    const std::string headerKey = getHeaderKey(header);
    const std::string headerValue = getHeaderValue(header);

    headers[headerKey] = trim(headerValue, " \t");
}
