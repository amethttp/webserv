#include "utils/string/string.hpp"
#include "TransferEncodingHeaderValidator.hpp"
#include "WebServer/Client/Request/Request.hpp"

bool TransferEncodingHeaderValidator::isValid(const headerValue_t &transferEncodingHeaderValues)
{
    if (transferEncodingHeaderValues.size() != 1)
        return false;

    const std::string transferEncodingValue = toLower(transferEncodingHeaderValues.front());

    return transferEncodingValue == "chunked";
}
