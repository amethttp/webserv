#pragma once

#include "utils/fileHandler/files.hpp"
#include "WebServer/Connection/Response/RequestHandler/RequestExecutor/Methods/AMethod.hpp"

class mDelete : public AMethod
{
private:
public:
	mDelete();
	~mDelete();

	t_HandlingResult execute(Context &ctx);
};
