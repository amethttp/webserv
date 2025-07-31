#include "Result.hpp"
#include <stdexcept>

template <typename T>
Result<T>::Result(T value, bool isSuccess, const t_httpCode error) : SimpleResult(isSuccess, error)
{
    this->value_ = value;
}

template <typename T>
Result<T>::~Result()
{
}

template <typename T>
Result<T> Result<T>::ok(T value)
{
    return Result<T>(value, true, 0);
}

template <typename T>
Result<T> Result<T>::fail(const t_httpCode error)
{
    return Result<T>(T(), false, error);
}

template <typename T>
T Result<T>::getValue() const
{
    if (isFailure())
        throw std::logic_error("Cannot access result value if the operation failed");

    return this->value_;
}
