#include "ReaderContext.hpp"

ReaderContext::ReaderContext() : scopeLevel_(0), line_(0), expectedScope_(false)
{
}

int ReaderContext::getScopeLevel()
{
	return this->scopeLevel_;
}

Config *ReaderContext::getCurrentConfig()
{
	return this->currConfig_;
}

int ReaderContext::incrScopeLevel()
{
	return ++this->scopeLevel_;
}

int ReaderContext::decrScopeLevel()
{
	return --this->scopeLevel_;
}

bool ReaderContext::getExpectedScope()
{
	return this->expectedScope_;
}

void ReaderContext::setExpectedScope(bool val)
{
	this->expectedScope_ = val;
}

int ReaderContext::getLine()
{
	return this->line_;
}


int ReaderContext::incrLine()
{
	return ++this->line_;
}


