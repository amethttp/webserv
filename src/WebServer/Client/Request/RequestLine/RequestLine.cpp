#include "RequestLine.hpp"

RequestLine::RequestLine()
{
    this->method_ = NOT_IMPLEMENTED;
}

RequestLine::~RequestLine() {}

method_t RequestLine::getMethod() const
{
    return this->method_;
}

Target_t & RequestLine::getTargetRef()
{
    return this->target_;
}

std::string RequestLine::getTargetUri() const
{
    return this->targetNew_.getUri();
}

std::string RequestLine::getTargetPath() const
{
    return this->targetNew_.getUriPath();
}

std::string RequestLine::getTargetQuery() const
{
    return this->targetNew_.getQueryPath();
}

std::string RequestLine::getHttpVersion() const
{
    return this->httpVersion_;
}

void RequestLine::setMethod(const std::string &methodString)
{
    if (methodString == "GET")
        this->method_ = GET;
    else if (methodString == "POST")
        this->method_ = POST;
    else if (methodString == "DELETE")
        this->method_ = DELETE;
    else
        this->method_ = NOT_IMPLEMENTED;
}

void RequestLine::setTargetUri(const std::string &uriString)
{
    this->target_.uri = uriString;
    this->targetNew_.setUri(uriString);
}

void RequestLine::setHttpVersion(const std::string &httpVersionString)
{
    this->httpVersion_ = httpVersionString;
}
