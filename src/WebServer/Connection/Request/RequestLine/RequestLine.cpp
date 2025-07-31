#include "RequestLine.hpp"

RequestLine::RequestLine()
{
    this->method_ = M_NOT_IMPLEMENTED;
}

RequestLine::~RequestLine() {}

t_method RequestLine::getMethod() const
{
    return this->method_;
}

std::string RequestLine::getTargetUri() const
{
    return this->target_.getUri();
}

std::string RequestLine::getTargetPath() const
{
    return this->target_.getUriPath();
}

std::string RequestLine::getTargetQuery() const
{
    return this->target_.getUriQuery();
}

std::string RequestLine::getHttpVersion() const
{
    return this->httpVersion_;
}

void RequestLine::setMethod(const std::string &methodString)
{
    if (methodString == "GET")
        this->method_ = M_GET;
    else if (methodString == "POST")
        this->method_ = M_POST;
    else if (methodString == "DELETE")
        this->method_ = M_DELETE;
    else
        this->method_ = M_NOT_IMPLEMENTED;
}

void RequestLine::setTargetUri(const std::string &uriString)
{
    this->target_.setUri(uriString);
}

void RequestLine::setHttpVersion(const std::string &httpVersionString)
{
    this->httpVersion_ = httpVersionString;
}
