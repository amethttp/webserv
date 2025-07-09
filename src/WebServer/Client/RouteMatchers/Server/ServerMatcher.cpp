#include "ServerMatcher.hpp"

Server ServerMatcher::matchServer(Request &request, std::vector<Server> &serverList)
{
    std::string hostName = request.getHeaders()["Host"];

    for (std::vector<Server>::iterator serverIt = serverList.begin(); serverIt != serverList.end(); ++serverIt)
    {
        if (serverIt->matchesName(hostName))
            return *serverIt;
    }

    return *serverList.begin();
}
