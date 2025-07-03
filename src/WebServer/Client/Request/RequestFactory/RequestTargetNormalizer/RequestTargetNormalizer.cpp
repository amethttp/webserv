#include "RequestTargetNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestTargetNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

pathComponents_t RequestTargetNormalizer::getPathComponents(const std::string &path)
{
    pathComponents_t pathComponents = split(path, "/");
    pathComponents.erase(pathComponents.begin());

    return pathComponents;
}

pathComponents_t RequestTargetNormalizer::normalizePathComponents(const pathComponents_t &pathComponents)
{
    pathComponents_t normalizedPathComponents;

    for (pathComponents_t::const_iterator it = pathComponents.begin(); it != pathComponents.end(); ++it)
    {
        if (*it == "." || (it != pathComponents.end() - 1 && *it == ""))
            continue;
        if (*it != "..")
            normalizedPathComponents.push_back(*it);
        else if (!normalizedPathComponents.empty())
            normalizedPathComponents.pop_back();
    }

    return normalizedPathComponents;
}

std::string RequestTargetNormalizer::buildNormalizedPath(const pathComponents_t &normalizedPathComponents)
{
    std::string normalizedPath = "/";

    for (pathComponents_t::const_iterator it = normalizedPathComponents.begin(); it != normalizedPathComponents.end(); ++it)
    {
        normalizedPath += *it;
        if (it != normalizedPathComponents.end() - 1)
            normalizedPath += "/";
    }

    return normalizedPath;
}

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    if (hasTrailingDotSegment(path))
        path += '/';

    const pathComponents_t pathComponents = getPathComponents(path);
    const pathComponents_t normalizedPathComponents = normalizePathComponents(pathComponents);
    path = buildNormalizedPath(normalizedPathComponents);
}
