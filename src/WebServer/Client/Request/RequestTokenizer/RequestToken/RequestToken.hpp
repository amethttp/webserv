#pragma once

#include <string>

#define METHOD "METHOD"
#define EOF "EOF"

class RequestToken
{
private:
    std::string type_;
    std::string value_;

public:
    RequestToken(const std::string &type, const std::string &value);
    ~RequestToken();

    std::string getType();
    std::string getValue();

    std::string toString() const;
};
