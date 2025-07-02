#include "RequestTargetNormalizer.hpp"

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    size_t currentPathPos = std::string::npos;
    do
    {
        currentPathPos = path.find("/.");

        if (currentPathPos != std::string::npos)
            path.replace(currentPathPos, 2, "");
    } while (currentPathPos != std::string::npos);

    if (path.empty())
        path = "/";
}
