#include "RequestTargetNormalizer.hpp"

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    const size_t currentPathPos = path.find("/.");

    if (currentPathPos != std::string::npos)
        path.erase(currentPathPos);
    if (path.empty())
        path = "/";
}
