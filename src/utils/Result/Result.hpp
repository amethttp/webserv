#pragma once

#include <string>
#include "utils/http/http.hpp"

class SimpleResult
{
protected:
    bool isSuccess_;
    t_httpCode error_;

    SimpleResult(bool isSuccess, const t_httpCode error);

public:
    ~SimpleResult();

    static SimpleResult ok();
    static SimpleResult fail(const t_httpCode error);

    bool isSuccess() const;
    bool isFailure() const;
    t_httpCode getError() const;
};

template <typename T>
class Result : public SimpleResult
{
private:
    T value_;

    Result(T value, bool isSuccess, const t_httpCode error);

public:
    ~Result();

    static Result<T> ok(T value);
    static Result<T> fail(const t_httpCode error);

    T getValue() const;
};

#include "Result.tpp"
