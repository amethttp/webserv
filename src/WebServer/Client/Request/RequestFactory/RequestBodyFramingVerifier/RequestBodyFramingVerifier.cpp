#include "RequestBodyFramingVerifier.hpp"

RequestBodyFramingVerifier::RequestBodyFramingVerifier(const std::string &bodyString)
{
    this->text = bodyString;
    this->pos = 0;
}

RequestBodyFramingVerifier::~RequestBodyFramingVerifier() {}

bool RequestBodyFramingVerifier::isFullBodyComplete(const size_t &contentLengthSize) const
{
    return this->text.length() >= contentLengthSize;
}
