#pragma once

#include "utils/http.hpp"
#include <string>

typedef struct RequestLineParams_s
{
    method_t method;
    std::string target;
    std::string httpVersion;
} RequestLineParams_t;
