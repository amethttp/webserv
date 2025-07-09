#include "utils/numeric/numeric.hpp"
#include "ContentLengthHeaderValidator.hpp"
#include <cstdlib>

bool ContentLengthHeaderValidator::isValid(const headerValue_t &contentLengthHeaderValues)
{
    if (contentLengthHeaderValues.size() != 1)
        return false;

    const std::string contentLengthValue = contentLengthHeaderValues.front();
    if (!isLong(contentLengthValue))
        return false;

    const long contentLengthSize = std::atol(contentLengthValue.c_str());
    return contentLengthSize >= 0;
}
