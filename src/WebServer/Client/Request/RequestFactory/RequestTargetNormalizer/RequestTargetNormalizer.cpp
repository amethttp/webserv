#include "RequestTargetNormalizer.hpp"

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    if (path[path.length() - 1] == '.')
        path.erase(path.length() - 1, 1);
}
