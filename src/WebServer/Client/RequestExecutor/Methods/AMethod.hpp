#pragma once

#include "utils/http.hpp"
#include "Context/Context.hpp"
#include "Result/ExecutionResult.hpp"

class AMethod
{
protected:
	t_method type_;
public:
	virtual ~AMethod();

	t_method const & getType() const;
	virtual ExecutionResult execute(Context &ctx) = 0;
};