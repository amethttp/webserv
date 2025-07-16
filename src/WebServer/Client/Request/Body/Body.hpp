#pragma once

#include <string>

class Body
{
private:
    std::string message_;

public:
    Body();
    ~Body();

    std::string getMessage();

    void addFragment(const std::string &fragment);
};
