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

void RequestExecutor::setBodyFromString(std::string str)
{
	this->body_.content = str;
	this->body_.type = this->extensionTypesDict_[".txt"];
}

ExecutionResult RequestExecutor::executeRequest(Context &ctx)
{
	AMethod *method;
	ExecutionResult result;
	MethodFactory mFactory(ctx.location_.getMethods());

	if (matchCGI(ctx.targetPath_, ctx.location_, cgi)) // keep only this here
	{
		if (this->executeCGI(ctx, cgi) == OK) // to each method ...
		{
			// check correctness?? || parse request
			result.isCGI_ = true;
			return OK;
		}
	}

	method = mFactory.create(ctx.method_);
	if (!method)
		result.code = METHOD_NOT_ALLOWED;
	else
	{
		result = method.execute(ctx);
		delete method;
	}
	
	return result;
}
