#pragma once

#include "Target/Target.hpp"
#include "utils/http/http.hpp"
#include <string>

class RequestLine
{
private:
    t_method method_;
    Target target_;
    std::string httpVersion_;

public:
    RequestLine();
    ~RequestLine();

    t_method getMethod() const;
    std::string getTargetUri() const;
    std::string getTargetPath() const;
    std::string getTargetQuery() const;
    std::string getHttpVersion() const;

    void setMethod(const std::string &methodString);
    void setTargetUri(const std::string &uriString);
    void setHttpVersion(const std::string &httpVersionString);
};
