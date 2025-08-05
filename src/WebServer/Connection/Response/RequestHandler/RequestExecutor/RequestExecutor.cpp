#include <iostream>
#include "RequestExecutor.hpp"

t_HandlingResult RequestExecutor::executeRequest(const Context &ctx)
{
	AMethod *method;
	t_HandlingResult result;
	MethodFactory mFactory(ctx.getLocation().getMethods());

	method = mFactory.create(ctx.getMethod());
	if (!method)
	{
		result.code_ = METHOD_NOT_ALLOWED;
		result.mode_ = ctx.getConnectionMode(); // check this
	}
	else
	{
		result = method->execute(ctx);
		delete method;
	}
	
	return result;
}
