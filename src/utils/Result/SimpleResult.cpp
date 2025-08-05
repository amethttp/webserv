#include "Result.hpp"
#include <stdexcept>

SimpleResult::SimpleResult(const bool isSuccess, const t_httpCode error)
{
    if (!isSuccess && !error)
        throw std::invalid_argument("A failing result must have a valid error message");

    this->isSuccess_ = isSuccess;
    this->error_ = error;
}

SimpleResult::~SimpleResult()
{
}

SimpleResult SimpleResult::ok()
{
    return SimpleResult(true, 0);
}

SimpleResult SimpleResult::fail(const t_httpCode error)
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

t_httpCode SimpleResult::getError() const
{
    return this->error_;
}
