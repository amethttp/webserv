#include "RequestPctDecoder.hpp"
#include "utils/numeric/numeric.hpp"

std::string RequestPctDecoder::decode(const std::string &string)
{
    std::string decodedString;

    for (size_t i = 0; i < string.length(); i++)
    {
        if (string[i] != '%')
        {
            decodedString += string[i];
            continue ;
        }

        decodedString += hexToChar(string[i + 1], string[i + 2]);
        i += 2;
    }

    return decodedString;
}

bool RequestPctDecoder::isWellEncoded(const std::string &string)
{
    for (size_t i = 0; i < string.length(); i++)
    {
        if (string[i] != '%')
            continue;

        const char decodedChar = hexToChar(string[i + 1], string[i + 2]);

        if (std::iscntrl(decodedChar))
            return false;
    }

    return true;
}