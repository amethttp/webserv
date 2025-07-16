#pragma once

#include "utils/fileHandler/files.hpp"
#include "WebServer/Client/RequestHandler/RequestExecutor/Methods/AMethod.hpp"

class mPost : public AMethod
{
private:
public:
	mPost();
	~mPost();

	HandlingResult execute(Context &ctx);
};
