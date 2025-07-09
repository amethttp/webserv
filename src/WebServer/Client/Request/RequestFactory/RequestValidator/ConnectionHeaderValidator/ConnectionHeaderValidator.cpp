#include "utils/string/string.hpp"
#include "ConnectionHeaderValidator.hpp"

bool ConnectionHeaderValidator::isValid(const headerValue_t &connectionHeaderValues)
{
    if (connectionHeaderValues.size() != 1)
        return false;

    const std::string connectionHeaderValue = toLower(connectionHeaderValues.front());

    return (connectionHeaderValue == "keep-alive" || connectionHeaderValue == "close");
}
