#include "RequestPctDecoder.hpp"
#include "utils/numeric/numeric.hpp"

bool RequestPctDecoder::isValidPctEncoded(const char firstDigit, const char secondDigit)
{
    return (std::isxdigit(firstDigit) && std::isxdigit(secondDigit));
}

Result<std::string> RequestPctDecoder::decode(const std::string &string)
{
    std::string decodedPath;

    for (size_t i = 0; i < string.length(); i++)
    {
        if (string[i] != '%')
        {
            decodedPath += string[i];
            continue ;
        }

        if (i + 2 >= string.length()
            || !isValidPctEncoded(string[i + 1], string[i + 2]))
            return Result<std::string>::fail("400 Bad Request");

        const char decodedChar = hexToChar(string[i + 1], string[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<std::string>::fail("400 Bad Request");

        decodedPath += decodedChar;
        i += 2;
    }

    return Result<std::string>::ok(decodedPath);
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