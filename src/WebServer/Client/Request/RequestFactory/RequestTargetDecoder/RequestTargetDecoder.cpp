#include "RequestTargetDecoder.hpp"
#include "utils/numeric/numeric.hpp"

Result<std::string> RequestTargetDecoder::decodePath(const std::string &path)
{
    std::string decodedPath;

    for (size_t i = 0; i < path.length(); i++)
    {
        if (path[i] != '%')
        {
            decodedPath += path[i];
            continue ;
        }

        const char decodedChar = hexToChar(path[i + 1], path[i + 2]);

        if (std::iscntrl(decodedChar))
            return Result<std::string>::fail("400 Bad Request");

        decodedPath += decodedChar;
        i += 2;
    }

    return Result<std::string>::ok(decodedPath);
}

bool RequestTargetDecoder::isEncodedQueryValid(const std::string &query)
{
    for (size_t i = 0; i < query.length(); i++)
    {
        if (query[i] != '%')
            continue;

        const char decodedChar = hexToChar(query[i + 1], query[i + 2]);

        if (std::iscntrl(decodedChar))
            return false;
    }

    return true;
}