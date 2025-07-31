#include "Exceptions.hpp"

const char *FatalException::what(char *msg) const throw()
{
    return (msg);
}

const char *ConnectionException::what(char *msg) const throw()
{
    return (msg);
}