#pragma once

#include "Methods/AMethod.hpp"
#include "utils/fileHandler/files.hpp"

class mDelete : public AMethod
{
private:
public:
	mDelete();
	~mDelete();

	HandlingResult execute(Context &ctx);
};