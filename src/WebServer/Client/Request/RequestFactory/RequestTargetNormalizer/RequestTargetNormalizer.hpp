#pragma once
#include <string>
#include <vector>

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

    static bool hasTrailingDotSegment(const std::string &path);
    static std::vector<std::string> normalizePathComponents(const std::vector<std::string> &pathComponents);
    static std::string buildNewPath(const std::vector<std::string> &newPathComponents);

public:

    static void normalizePath(std::string &path);
};
