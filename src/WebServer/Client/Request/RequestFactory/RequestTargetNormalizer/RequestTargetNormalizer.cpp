#include "RequestTargetNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestTargetNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    if (hasTrailingDotSegment(path))
        path += '/';

    std::vector<std::string> newPathComponents;
    std::vector<std::string> pathComponents = split(path, "/");

    for (std::vector<std::string>::iterator it = pathComponents.begin() + 1; it != pathComponents.end(); ++it)
    {
        if (*it == ".")
            continue;

        newPathComponents.push_back(*it);
    }

    std::string newPath = "/";
    for (std::vector<std::string>::iterator it = newPathComponents.begin(); it != newPathComponents.end(); ++it)
    {
        newPath += *it;

        if (it != newPathComponents.end() - 1)
            newPath += "/";
    }

    path = newPath;
}
