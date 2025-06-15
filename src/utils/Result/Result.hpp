#pragma once

#include <string>

template<typename T>
class Result
{
private:
    bool isSuccess_;
    std::string error_;
    T value_;

    Result(T value, bool isSuccess, std::string error);

public:
    ~Result();

    static Result<T> ok(T value);
    static Result<T> fail(std::string error);

    bool isSuccess();
    bool isFailure();
    std::string getError();
    T getValue();
};

#include "Result.tpp"
