#include <iostream>
#include "RequestExecutor.hpp"

HandlingResult RequestExecutor::executeRequest(Context &ctx)
{
	AMethod *method;
	HandlingResult result;
	MethodFactory mFactory(ctx.location_.getMethods());

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
