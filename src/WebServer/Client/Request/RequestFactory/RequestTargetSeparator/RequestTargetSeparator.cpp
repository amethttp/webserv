#include "RequestTargetSeparator.hpp"
#include <algorithm>

void RequestTargetSeparator::separateInComponents(Target_t &target)
{
    const std::string::iterator uriBegin = target.uri.begin();
    const std::string::iterator uriEnd = target.uri.end();
    const std::string::iterator queryStart = std::find(uriBegin, uriEnd, '?');

    target.path = std::string(uriBegin, queryStart);
    if (queryStart != uriEnd)
        target.query = std::string(queryStart + 1, uriEnd);
}
