#include "RequestTargetNormalizer.hpp"

void RequestTargetNormalizer::normalizePath(std::string &path)
{
    size_t currentPathPos = std::string::npos;
    do
    {
        currentPathPos = path.find("/.");

        if (currentPathPos != std::string::npos)
            path.replace(currentPathPos, 2, "/");
    } while (currentPathPos != std::string::npos);

    if (path.empty())
        path = "/";

    bool itsSlash = false;
    for (size_t i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            if (itsSlash)
            {
                path.replace(i, 1, "");
                i--;
            }
            itsSlash = true;
        }
        else
            itsSlash = false;
    }
}
