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
    method_t method;
    Target_t target;
    std::string httpVersion;

public:
    RequestLine();
    ~RequestLine();
};

typedef struct RequestLineParams_s
{
    method_t method;
    Target_t target;
    std::string httpVersion;
} RequestLineParams_t;
