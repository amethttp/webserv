#pragma once

#include <string>

typedef enum tokenType_e
{
    METHOD,
    SP,
    TARGET,
    HTTP_VERSION,
    HEADER,
    CHUNK,
    LAST_CHUNK,
    CRLF,
    EOF,
    UNKNOWN,
} tokenType_t;

class RequestToken
{
private:
    tokenType_t type_;
    std::string value_;

public:
    RequestToken(tokenType_t type, const std::string &value);
    ~RequestToken();

    tokenType_t getType() const;
    std::string getValue();

    std::string toString() const;
};
