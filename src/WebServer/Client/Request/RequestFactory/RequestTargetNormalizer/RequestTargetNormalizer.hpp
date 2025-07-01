#pragma once
#include <string>

class RequestTargetNormalizer
{
private:
    RequestTargetNormalizer();

public:
    static void normalizePath(std::string &path);
};
