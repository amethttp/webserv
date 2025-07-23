#pragma once

#include "../../Config.hpp"

class ReaderContext
{
private:
	int scopeLevel_;
	int line_;
	bool expectedScope_;
	Config *currConfig_;

public:
	ReaderContext();
	int getScopeLevel();
	int incrScopeLevel();
	int decrScopeLevel();
	bool getExpectedScope();
	void setExpectedScope(bool);
	int getLine();
	int incrLine();
	Config *getCurrentConfig();
};