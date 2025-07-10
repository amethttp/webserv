#include "HostHeaderValidator.hpp"
#include "utils/numeric/numeric.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include <cstdlib>

bool HostHeaderValidator::isUnreserved(const char c)
{
    const std::string unreservedSymbols = "-._~";

    return (std::isalnum(c) || unreservedSymbols.find(c) != std::string::npos);
}

bool HostHeaderValidator::isPctEncoded(const std::string &header, const size_t pos)
{
    if (pos + 2 >= header.length())
        return false;

    return (header[pos] == '%'
            && std::isxdigit(header[pos + 1])
            && std::isxdigit(header[pos + 2]));
}

bool HostHeaderValidator::isSubDelim(const char c)
{
    const std::string subDelimSymbols = "!$&'()*+,;=";

    return subDelimSymbols.find(c) != std::string::npos;
}

bool HostHeaderValidator::isRegName(const std::string &header, const size_t pos)
{
    return (isUnreserved(header[pos]) || isPctEncoded(header, pos) || isSubDelim(header[pos]));
}

bool HostHeaderValidator::isValidHostName(const std::string &hostName)
{
    if (hostName.empty())
        return false;

    for (size_t i = 0; i < hostName.length(); i++)
    {
        if (!isRegName(hostName, i))
            return false;
    }

    return true;
}

bool HostHeaderValidator::isValidHostPort(const std::string &port)
{
    if (port.empty() || !isLong(port))
        return false;

    const long portNum = std::atol(port.c_str());

    return (portNum >= 0 && portNum <= 65535);
}

bool HostHeaderValidator::isValid(const Header &hostHeader)
{
    if (hostHeader.getAmountOfValues() != 1)
        return false;

    const std::string hostValue = hostHeader.getValue();
    const size_t portSeparator = hostValue.find(':');
    const std::string hostName = hostValue.substr(0, portSeparator);
    std::string hostPort;

    if (portSeparator != std::string::npos)
        hostPort = hostValue.substr(portSeparator + 1);

    if (!isValidHostName(hostName))
        return false;
    if (portSeparator != std::string::npos)
        return isValidHostPort(hostPort);

    return true;
}
