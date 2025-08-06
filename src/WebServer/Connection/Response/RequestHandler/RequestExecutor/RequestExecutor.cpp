#include <iostream>
#include "RequestExecutor.hpp"
#include "utils/exceptions/Exceptions.hpp"

t_HandlingResult RequestExecutor::executeRequest(Context &ctx)
{
	AMethod *method;
	t_HandlingResult result;
	MethodFactory mFactory(ctx.getLocation().getMethods());

	method = mFactory.create(ctx.getMethod());
	if (!method)
	{
		result.code_ = METHOD_NOT_ALLOWED;
		result.mode_ = ctx.getConnectionMode();
	}
	else
	{
		try
		{
			result = method->execute(ctx);
			delete method;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			delete method;
		}		
	}
	
	return result;
}
