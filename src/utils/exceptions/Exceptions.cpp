#include "Exceptions.hpp"

FatalException::FatalException(const char *msg)
{
    this->exceptionMsg = "Fatal Exception: <" + static_cast<std::string>(msg) + ">";
}

FatalException::~FatalException() throw()
{
}

const char *FatalException::what() const throw()
{
    return (this->exceptionMsg.c_str());
}

RecoverableException::RecoverableException(const char *msg)
{
    this->exceptionMsg = "Recoverable Exception: <" + static_cast<std::string>(msg) + ">";
}

RecoverableException::~RecoverableException() throw()
{
}

const char *RecoverableException::what() const throw()
{
    return (this->exceptionMsg.c_str());
}
