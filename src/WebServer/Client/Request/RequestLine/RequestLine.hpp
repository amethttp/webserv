#pragma once

#include "Target/Target.hpp"
#include "utils/HTTP/http.hpp"
#include <string>

typedef struct Target_s
{
    std::string uri;
    std::string path;
    std::string query;
} Target_t;

class RequestLine
{
private:
    method_t method_;
    Target_t target_;
    Target targetNew_;
    std::string httpVersion_;

public:
    RequestLine();
    ~RequestLine();

    method_t getMethod() const;
    Target_t &getTargetRef();
    std::string getTargetUri() const;
    std::string getTargetPath() const;
    std::string getTargetQuery() const;
    std::string getHttpVersion() const;

    void setMethod(const std::string &methodString);
    void setTargetUri(const std::string &uriString);
    void setHttpVersion(const std::string &httpVersionString);
};
