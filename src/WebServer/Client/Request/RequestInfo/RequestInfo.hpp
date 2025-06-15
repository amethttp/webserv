#pragma once

#include "../Request.hpp"

typedef struct RequestInfo_s
{
    Request_t request;
    bool canCreateAResponse;
} RequestInfo_t;
