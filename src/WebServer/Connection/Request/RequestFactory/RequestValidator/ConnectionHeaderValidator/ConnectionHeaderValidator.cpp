#include "utils/string/string.hpp"
#include "ConnectionHeaderValidator.hpp"

bool ConnectionHeaderValidator::isValid(const Header &connectionHeader)
{
    if (connectionHeader.getAmountOfValues() != 1)
        return false;

    const std::string connectionHeaderValue = toLower(connectionHeader.getValue());

    return (connectionHeaderValue == "keep-alive" || connectionHeaderValue == "close");
}
