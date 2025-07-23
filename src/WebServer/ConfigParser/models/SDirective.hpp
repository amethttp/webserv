#pragma once


struct SDirective
{
	int argMin;
	int argMax;
	bool isContainer;

	SDirective(int min, int max, bool block)
		: argMin(min), argMax(max), isContainer(block) {}

	SDirective() : argMin(0), argMax(0), isContainer(false) {}
};