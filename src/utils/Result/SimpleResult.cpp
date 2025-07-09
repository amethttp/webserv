#include "Result.hpp"
#include <stdexcept>

SimpleResult::SimpleResult(const bool isSuccess, const std::string &error)
{
    if (!isSuccess && error.empty())
        throw std::invalid_argument("A failing result must have a valid error message");

    this->isSuccess_ = isSuccess;
    this->error_ = error;
}

SimpleResult::~SimpleResult()
{
}

SimpleResult SimpleResult::ok()
{
    return SimpleResult(true, "");
}

SimpleResult SimpleResult::fail(const std::string &error)
{
    return SimpleResult(false, error);
}

bool SimpleResult::isSuccess() const
{
    return this->isSuccess_;
}

bool SimpleResult::isFailure() const
{
    return !this->isSuccess_;
}

std::string SimpleResult::getError() const
{
    return this->error_;
}
