#pragma once

#include <string>
#include <exception>

class TestFailure : public std::exception
{
private:
    std::string message_;

public:
    TestFailure(const std::string &message);
    ~TestFailure() throw();

    const char *what() const throw();
};
