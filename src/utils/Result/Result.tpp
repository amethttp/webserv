#include "Result.hpp"
#include <stdexcept>

template<typename T>
Result<T>::Result(T value, bool isSuccess, std::string error)
{
    this->value_ = value;
    this->isSuccess_ = isSuccess;
    this->error_ = error;
}

template<typename T>
Result<T>::~Result()
{
}

template<typename T>
Result<T> Result<T>::ok(T value)
{
    return Result<T>(value, true, "");
}

template<typename T>
Result<T> Result<T>::fail(std::string error)
{
    return Result<T>(T(), false, error);
}

template<typename T>
bool Result<T>::isSuccess()
{
    return this->isSuccess_;
}

template<typename T>
bool Result<T>::isFailure()
{
    return !this->isSuccess_;
}

template<typename T>
std::string Result<T>::getError()
{
    return this->error_;
}

template<typename T>
T Result<T>::getValue()
{
    return this->value_;
}
