#pragma once

#include <string>
#include <vector>

typedef std::vector<std::string> pathComponents_t;

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

    static bool hasTrailingDotSegment(const std::string &path);
    static pathComponents_t getPathComponents(const std::string &path);
    static pathComponents_t normalizePathComponents(const pathComponents_t &pathComponents);
    static std::string buildNormalizedPath(const pathComponents_t &normalizedPathComponents);

public:
    static void normalizePath(std::string &path);
};
