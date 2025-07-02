#include "RequestTargetNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestTargetNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

pathComponents_t RequestTargetNormalizer::normalizePathComponents(const pathComponents_t &pathComponents)
{
    pathComponents_t normalizedPathComponents;

    for (pathComponents_t::const_iterator it = pathComponents.begin() + 1; it != pathComponents.end(); ++it)
    {
        if (*it == ".")
            continue;
        normalizedPathComponents.push_back(*it);
    }

    return normalizedPathComponents;
}

std::string RequestTargetNormalizer::buildNewPath(const pathComponents_t &newPathComponents)
{
    std::string newPath = "/";

    for (pathComponents_t::const_iterator it = newPathComponents.begin(); it != newPathComponents.end(); ++it)
    {
        newPath += *it;
        if (it != newPathComponents.end() - 1)
            newPath += "/";
    }

    return newPath;
}

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    if (hasTrailingDotSegment(path))
        path += '/';

    const pathComponents_t pathComponents = split(path, "/");
    const pathComponents_t newPathComponents = normalizePathComponents(pathComponents);
    path = buildNewPath(newPathComponents);
}
