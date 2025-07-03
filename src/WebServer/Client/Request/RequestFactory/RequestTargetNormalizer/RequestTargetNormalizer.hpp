#pragma once

#include <string>
#include <vector>

typedef std::vector<std::string> pathSegments_t;

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

    static bool isLastElement(const pathSegments_t &pathSegments, pathSegments_t::const_iterator it);

    static bool hasTrailingDotSegment(const std::string &path);
    static pathSegments_t getPathSegments(const std::string &path);
    static pathSegments_t normalizePathSegments(const pathSegments_t &pathSegments);
    static std::string buildNormalizedPath(const pathSegments_t &normalizedPathSegments);

public:
    static void normalizePath(std::string &path);
};
