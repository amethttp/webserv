#include "utils/string/string.hpp"
#include "TransferEncodingHeaderValidator.hpp"
#include "WebServer/Client/Request/Request.hpp"

bool TransferEncodingHeaderValidator::isValid(const Header &transferEncodingHeader)
{
    if (transferEncodingHeader.getAmountOfValues() != 1)
        return false;

    const std::string transferEncodingValue = toLower(transferEncodingHeader.getValue());

    return transferEncodingValue == "chunked";
}
