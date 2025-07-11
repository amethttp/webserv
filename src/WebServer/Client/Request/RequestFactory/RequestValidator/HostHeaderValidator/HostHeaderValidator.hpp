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

    static bool hasPort(const std::string &hostValue);

    static std::string getHostName(const std::string &hostValue);
    static std::string getHostPort(const std::string &hostValue);

public:
    static bool isValid(const Header &hostHeader);
};
