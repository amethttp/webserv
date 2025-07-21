#pragma once

#include <string>
#include "utils/http.hpp"
#include "utils/cgi/cgi.hpp"
#include "Methods/AMethod.hpp"
#include "Methods/Factory/MethodFactory.hpp"

class RequestExecutor
{
private:

public:
	static HandlingResult executeRequest(Context &c);
};
