#pragma once

#include "Target/Target.hpp"
#include "utils/HTTP/http.hpp"
#include <string>

class RequestLine
{
private:
    method_t method_;
    Target target_;
    std::string httpVersion_;

public:
    RequestLine();
    ~RequestLine();

    method_t getMethod() const;
    std::string getTargetUri() const;
    std::string getTargetPath() const;
    std::string getTargetQuery() const;
    std::string getHttpVersion() const;

    void setMethod(const std::string &methodString);
    void setTargetUri(const std::string &uriString);
    void setTargetPath(const std::string &pathString);
    void setHttpVersion(const std::string &httpVersionString);
};
