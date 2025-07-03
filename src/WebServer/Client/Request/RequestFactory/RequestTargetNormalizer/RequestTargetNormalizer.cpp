#include "RequestTargetNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestTargetNormalizer::isLastElement(const pathSegments_t &pathSegments, const pathSegments_t::const_iterator it)
{
    return it == pathSegments.end() - 1;
}

bool RequestTargetNormalizer::isCurrentDirectory(const std::string &str)
{
    return str == ".";
}

bool RequestTargetNormalizer::isEmptySegment(const pathSegments_t &pathSegments, const pathSegments_t::const_iterator it)
{
    return (!isLastElement(pathSegments, it) && *it == "");
}

bool RequestTargetNormalizer::isParentDirectory(const std::string &str)
{
    return str == "..";
}

bool RequestTargetNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

pathSegments_t RequestTargetNormalizer::getPathSegments(const std::string &path)
{
    pathSegments_t pathSegments = split(path, "/");
    pathSegments.erase(pathSegments.begin());

    return pathSegments;
}

pathSegments_t RequestTargetNormalizer::normalizePathSegments(const pathSegments_t &pathSegments)
{
    pathSegments_t normalizedPathSegments;

    for (pathSegments_t::const_iterator it = pathSegments.begin(); it != pathSegments.end(); ++it)
    {
        if (isCurrentDirectory(*it) || isEmptySegment(pathSegments, it))
            continue;
        if (!isParentDirectory(*it))
            normalizedPathSegments.push_back(*it);
        else if (!normalizedPathSegments.empty())
            normalizedPathSegments.pop_back();
    }

    return normalizedPathSegments;
}

std::string RequestTargetNormalizer::buildNormalizedPath(const pathSegments_t &normalizedPathSegments)
{
    std::string normalizedPath = "/";

    for (pathSegments_t::const_iterator it = normalizedPathSegments.begin(); it != normalizedPathSegments.end(); ++it)
    {
        normalizedPath += *it;
        if (!isLastElement(normalizedPathSegments, it))
            normalizedPath += "/";
    }

    return normalizedPath;
}

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    if (hasTrailingDotSegment(path))
        path += '/';

    const pathSegments_t pathSegments = getPathSegments(path);
    const pathSegments_t normalizedPathSegments = normalizePathSegments(pathSegments);
    path = buildNormalizedPath(normalizedPathSegments);
}
