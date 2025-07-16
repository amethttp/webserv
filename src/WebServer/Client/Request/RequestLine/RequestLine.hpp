#pragma once

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
    std::string httpVersion_;

public:
    RequestLine();
    ~RequestLine();

    method_t getMethod() const;
    std::string getTargetUri() const;
    std::string getHttpVersion() const;

        void setMethod(method_t method);
    void setMethod(const std::string &methodString);
    void setTargetUri(const std::string &uriString);
    void setHttpVersion(const std::string &httpVersionString);
};

typedef struct RequestLineParams_s
{
    method_t method;
    Target_t target;
    std::string httpVersion;
} RequestLineParams_t;
