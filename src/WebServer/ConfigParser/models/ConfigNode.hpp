#pragma once
#include <string>
#include <vector>

struct ConfigNode
{
	std::string name;
	std::vector<std::string> params;
	std::vector<ConfigNode> children;

	bool operator<(const ConfigNode other) const
	{
		return name < other.name;
	}
};

struct Frame
{
	std::vector<ConfigNode> *container;
	ConfigNode current;
	bool building;
};
