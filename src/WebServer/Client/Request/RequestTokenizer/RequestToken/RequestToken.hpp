#pragma once

#include <string>

typedef enum e_tokenType
{
    METHOD,
    SP,
    TARGET,
    HTTP_VERSION,
    HEADER,
    CHUNK_SIZE,
    LAST_CHUNK,
    OCTET_STREAM,
    CRLF,
    TT_EOF,
    UNKNOWN,
    EMPTY
} t_tokenType;

class RequestToken
{
private:
    t_tokenType type_;
    std::string value_;

public:
    RequestToken(t_tokenType type, const std::string &value);
    ~RequestToken();

    t_tokenType getType() const;
    std::string getValue();

    std::string toString() const;
};
