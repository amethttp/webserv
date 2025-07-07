#include "http.hpp"

method_t getHttpMethodFromString(const std::string &requestMethodString)
{
    if (requestMethodString == "GET")
        return GET;
    if (requestMethodString == "POST")
        return POST;
    if (requestMethodString == "DELETE")
        return DELETE;
    return NOT_IMPLEMENTED;
}

std::string toHttpHeaderCase(const std::string &string)
{
    std::string result = string;

    result[0] = static_cast<char>(std::toupper(result[0]));
    for (size_t i = 1; i < result.length(); i++)
    {
        if (result[i - 1] == '-')
            result[i] = static_cast<char>(std::toupper(result[i]));
        else
            result[i] = static_cast<char>(std::tolower(result[i]));
    }

    return result;
}

std::string getHeaderKey(const std::string &header)
{
    const size_t headerKeyEnd = header.find(':');
    const std::string headerKey = header.substr(0, headerKeyEnd);

    return toHttpHeaderCase(headerKey);
}

std::string getHeaderValue(const std::string &header)
{
    const size_t headerValueStart = header.find(':') + 1;
    const std::string headerValue = header.substr(headerValueStart);

    return headerValue;
}
