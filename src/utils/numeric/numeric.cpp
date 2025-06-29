#include "numeric.hpp"
#include <sstream>

bool isLong(const std::string &str)
{
    long num;
    std::istringstream stream(str);

    stream >> std::noskipws >> num;
    return stream.eof() && !stream.fail();
}

bool isHex(const std::string &str)
{
    size_t num;
    std::istringstream stream(str);

    stream >> std::hex >> std::noskipws >> num;
    return stream.eof() && !stream.fail();
}

size_t hexToDec(const std::string &hexNumber)
{
    if (!isHex(hexNumber))
        throw std::invalid_argument("The string argument is not an hexadecimal number");

    size_t num;
    std::istringstream stream(hexNumber);

    stream >> std::hex >> num;
    return num;
}

char hexToChar(const char firstDigit, const char secondDigit)
{
    const std::string hexString = std::string(1, firstDigit) + std::string(1, secondDigit);

    const size_t result = hexToDec(hexString);

    return static_cast<char>(result);
}
