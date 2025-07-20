#include "RequestPathNormalizer.hpp"
#include "utils/string/string.hpp"
#include <vector>

bool RequestPathNormalizer::isLastElement(const pathSegments_t &pathSegments, const pathSegments_t::const_iterator it)
{
    return it == pathSegments.end() - 1;
}

bool RequestPathNormalizer::isCurrentDirectory(const std::string &str)
{
    return str == ".";
}

bool RequestPathNormalizer::isEmptySegment(const pathSegments_t &pathSegments, const pathSegments_t::const_iterator it)
{
    return (!isLastElement(pathSegments, it) && *it == "");
}

bool RequestPathNormalizer::isParentDirectory(const std::string &str)
{
    return str == "..";
}

void RequestPathNormalizer::removeLastElement(pathSegments_t &pathSegments)
{
    if (pathSegments.empty())
        return;

    pathSegments.pop_back();
}

bool RequestPathNormalizer::hasTrailingDotSegment(const std::string &path)
{
    return endsWith(path, "/.");
}

pathSegments_t RequestPathNormalizer::getPathSegments(const std::string &path)
{
    pathSegments_t pathSegments = split(path, "/");
    pathSegments.erase(pathSegments.begin());

    return pathSegments;
}

pathSegments_t RequestPathNormalizer::normalizePathSegments(const pathSegments_t &pathSegments)
{
    pathSegments_t normalizedPathSegments;

    for (pathSegments_t::const_iterator it = pathSegments.begin(); it != pathSegments.end(); ++it)
    {
        if (isCurrentDirectory(*it) || isEmptySegment(pathSegments, it))
            continue;
        if (isParentDirectory(*it))
            removeLastElement(normalizedPathSegments);
        else
            normalizedPathSegments.push_back(*it);
    }

    return normalizedPathSegments;
}

std::string RequestPathNormalizer::buildNormalizedPath(const pathSegments_t &normalizedPathSegments)
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

std::string RequestPathNormalizer::normalizePath(const std::string &path)
{
    std::string tempPath = path;

    if (hasTrailingDotSegment(path))
        tempPath += '/';

    const pathSegments_t pathSegments = getPathSegments(tempPath);
    const pathSegments_t normalizedPathSegments = normalizePathSegments(pathSegments);
    return buildNormalizedPath(normalizedPathSegments);
}
