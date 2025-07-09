#include "LocationMatcher.hpp"

namespace
{
	// Decide if we normalize slashes or not || Nginx default beaviour doesnt do it
	void removeTrailingSlashes(std::string &str)
	{
		if (*(str.rbegin()) == '/' && str.length() > 1)
			str.erase(str.length() - 1);
	}

	int countMatchingDepth(std::string base, std::string target)
	{
		size_t i = 0;
		size_t len = 0;

		if (base.length() > target.length())
			return -1;

		while (i < base.length())
		{
			if (base[i] != target[i])
				break ;
			i++;
		}

		if (i != base.length())
			return -1;
		else if ((base.length() != target.length()) && (base[i - 1] != '/'))
			return -1;

		return i;
	}
}

// Decide on no locations defined on server
Location LocationMatcher::matchLocation(Request &request, Server &server)
{
    int matchIndex = 0;
    int matchLength = 0;
    int longestMatch = 0;
    std::string targetRoute = request.getTarget();
    std::vector<Location> locations = server.getLocations();

    for (size_t i = 0; i < locations.size(); ++i)
    {
        matchLength = countMatchingDepth(locations[i].getPath(), targetRoute);
        if (matchLength > longestMatch)
        {
            matchIndex = i;
            longestMatch = matchLength;
        }
    }

    return locations[matchIndex];
}