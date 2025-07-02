#pragma once
#include <string>

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

    static bool hasTrailingDotSegment(const std::string &path);

public:
    static void normalizePath(std::string &path);
};
