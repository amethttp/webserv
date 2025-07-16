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
    return this->target_.uri;
}

std::string RequestLine::getTargetPath() const
{
    return this->target_.path;
}

std::string RequestLine::getTargetQuery() const
{
    return this->target_.query;
}

std::string RequestLine::getHttpVersion() const
{
    return this->httpVersion_;
}

void RequestLine::setMethod(const method_t method)
{
    this->method_ = method;
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
}

void RequestLine::setHttpVersion(const std::string &httpVersionString)
{
    this->httpVersion_ = httpVersionString;
}
