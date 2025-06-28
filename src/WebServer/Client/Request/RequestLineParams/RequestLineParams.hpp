#pragma once

#include "utils/HTTP/http.hpp"
#include <string>

typedef struct RequestLineParams_s
{
    method_t method;
    std::string target;
    std::string httpVersion;
} RequestLineParams_t;
