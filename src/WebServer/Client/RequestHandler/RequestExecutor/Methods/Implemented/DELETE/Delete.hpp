#pragma once

#include "utils/fileHandler/files.hpp"
#include "WebServer/Client/RequestHandler/RequestExecutor/Methods/AMethod.hpp"

class mDelete : public AMethod
{
private:
public:
	mDelete();
	~mDelete();

	HandlingResult execute(Context &ctx);
};
