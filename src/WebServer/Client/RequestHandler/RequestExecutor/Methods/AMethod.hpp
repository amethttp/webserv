#pragma once

#include "utils/http.hpp"
#include "utils/cgi/cgi.hpp"
#include "Context/Context.hpp"
#include "Result/HandlingResult.hpp"

class AMethod
{
protected:
	t_method type_;
public:
	virtual ~AMethod();

	t_method const & getType() const;
	virtual HandlingResult execute(Context &ctx) = 0;
};