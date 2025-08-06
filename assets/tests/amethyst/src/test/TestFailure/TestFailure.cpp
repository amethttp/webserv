#include "TestFailure.hpp"
#include <sstream>

TestFailure::TestFailure(const std::string &message)
{
    this->message_ = message;
}

TestFailure::~TestFailure() throw()
{
}

const char *TestFailure::what() const throw()
{
    return this->message_.c_str();
}
