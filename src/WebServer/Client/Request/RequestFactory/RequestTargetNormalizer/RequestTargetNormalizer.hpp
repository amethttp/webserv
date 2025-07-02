#pragma once
#include <string>
#include <vector>

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

    static bool hasTrailingDotSegment(const std::string &path);
    static std::vector<std::string> normalizePathComponents(const std::vector<std::string> &pathComponents);

public:
    static void normalizePath(std::string &path);
};
