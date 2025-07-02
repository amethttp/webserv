#include "RequestTargetNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestTargetNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

std::vector<std::string> RequestTargetNormalizer::normalizePathComponents(const std::vector<std::string> &pathComponents)
{
    std::vector<std::string> processedPathComponents;

    for (std::vector<std::string>::const_iterator it = pathComponents.begin() + 1; it != pathComponents.end(); ++it)
    {
        if (*it == ".")
            continue;
        processedPathComponents.push_back(*it);
    }

    return processedPathComponents;
}

std::string RequestTargetNormalizer::buildNewPath(const std::vector<std::string> &newPathComponents)
{
    std::string newPath = "/";

    for (std::vector<std::string>::const_iterator it = newPathComponents.begin(); it != newPathComponents.end(); ++it)
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

    const std::vector<std::string> pathComponents = split(path, "/");
    const std::vector<std::string> newPathComponents = normalizePathComponents(pathComponents);
    path = buildNewPath(newPathComponents);
}
