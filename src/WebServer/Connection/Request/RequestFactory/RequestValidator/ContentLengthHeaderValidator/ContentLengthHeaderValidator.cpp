#include "utils/numeric/numeric.hpp"
#include "ContentLengthHeaderValidator.hpp"
#include <cstdlib>

bool ContentLengthHeaderValidator::isValid(const Header &contentLengthHeader)
{
    if (contentLengthHeader.getAmountOfValues() != 1)
        return false;

    const std::string contentLengthValue = contentLengthHeader.getValue();
    if (!isLong(contentLengthValue))
        return false;

    const long contentLengthSize = std::atol(contentLengthValue.c_str());
    return contentLengthSize >= 0;
}
