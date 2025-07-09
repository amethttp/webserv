#pragma once

#include "WebServer/Client/Request/Request.hpp"
#include <string>

class HostHeaderValidator
{
private:
    static bool isUnreserved(char c);
    static bool isPctEncoded(const std::string &header, size_t pos);
    static bool isSubDelim(char c);
    static bool isRegName(const std::string &header, size_t pos);

    static bool isValidHostName(const std::string &hostName);
    static bool isValidHostPort(const std::string &port);

public:
    static bool isValid(const headerValue_t &hostHeaderValues);
};
