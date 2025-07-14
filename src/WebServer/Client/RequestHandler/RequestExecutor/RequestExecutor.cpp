#include <iostream>
#include "Response.hpp"
#include "RequestExecutor.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <unistd.h>

std::string RequestExecutor::getMIME(std::string &target)
{
	int pos;
	std::string res = "text/plain";

	pos = target.rfind('.');
	if (pos != std::string::npos)
	{
		if (this->extensionTypesDict_.find(target.substr(pos)) != this->extensionTypesDict_.end())
			res = this->extensionTypesDict_[target.substr(pos)];
	}
	
	return res;
}

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
