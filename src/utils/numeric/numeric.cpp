#include "numeric.hpp"
#include <sstream>

bool isLong(const std::string &str)
{
    long num;
    std::istringstream stream(str);

    stream >> std::noskipws >> num;
    return stream.eof() && !stream.fail();
}
