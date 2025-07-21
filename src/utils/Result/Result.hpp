#pragma once

#include <string>

class SimpleResult
{
protected:
    bool isSuccess_;
    std::string error_;

    SimpleResult(bool isSuccess, const std::string &error);

public:
    ~SimpleResult();

    static SimpleResult ok();
    static SimpleResult fail(const std::string &error);

    bool isSuccess() const;
    bool isFailure() const;
    std::string getError() const;
};

template <typename T>
class Result : public SimpleResult
{
private:
    T value_;

    Result(T value, bool isSuccess, const std::string &error);

public:
    ~Result();

    static Result<T> ok(T value);
    static Result<T> fail(const std::string &error);

    T getValue() const;
};

#include "Result.tpp"
