#pragma once
#include <map>
#include <string>

struct SDirective
{
	int argMin;
	int argMax;
	bool isContainer;

	SDirective(int min, int max, bool block)
		: argMin(min), argMax(max), isContainer(block) {}

	SDirective() : argMin(0), argMax(0), isContainer(false) {}
};

class DirectiveRegistry
{
private:
	static std::map<std::string, SDirective> directives_;

public:
	static std::map<std::string, SDirective> getDirectives();
	static SDirective getDirective(std::string &key);
};