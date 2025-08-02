#pragma once

#include "utils/fileHandler/files.hpp"
#include "WebServer/Client/Response/RequestHandler/RequestExecutor/Methods/AMethod.hpp"

class mPost : public AMethod
{
private:
public:
	mPost();
	~mPost();

	t_HandlingResult execute(Context &ctx);
};
