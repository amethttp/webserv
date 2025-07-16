#pragma once

#include "utils/http.hpp"
#include "utils/cgi/cgi.hpp"
#include "WebServer/Client/RequestHandler/Context/Context.hpp"
#include "WebServer/Client/RequestHandler/Result/HandlingResult.hpp"

class AMethod
{
protected:
	t_method type_;
public:
	virtual ~AMethod();

	t_method const & getType() const;
	virtual HandlingResult execute(Context &ctx) = 0;
};
