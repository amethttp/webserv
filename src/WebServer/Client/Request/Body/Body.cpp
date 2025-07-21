#include "Body.hpp"
#include "utils/numeric/numeric.hpp"
#include <algorithm>

Body::Body() {}

Body::~Body() {}

bool Body::isNotHex(const char c)
{
    return !std::isxdigit(c);
}

std::string Body::getMessage()
{
    return this->message_;
}

size_t Body::getChunkSize(const std::string &chunkSize)
{
    const std::string::const_iterator chunkSizeBegin = chunkSize.begin();
    const std::string::const_iterator chunkSizeEnd = chunkSize.end();
    const std::string::const_iterator chunkSizeValueEnd = std::find_if(chunkSizeBegin, chunkSizeEnd, isNotHex);
    const std::string chunkSizeValue = std::string(chunkSizeBegin, chunkSizeValueEnd);

    return hexToDec(chunkSizeValue);
}

void Body::addFragment(const std::string &fragment)
{
    this->message_ += fragment;
}
