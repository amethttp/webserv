#pragma once
#include <vector>
#include <iostream>
#include "WebServer/Server/Location/Location.hpp"
#include "WebServer/Server/Session/Session.hpp"

enum ConfigBlockType
{
	CONTEXT_BLOCK,
	SERVER_BLOCK,
	LOCATION_BLOCK
};

struct ConfigBlock
{
	ConfigBlockType type;
	std::set<int> ports_;
	std::vector<std::string> names_;
	std::string uploadPath_;
	std::string path_;
	std::string root_;
	std::map<std::string, std::string> cgis_;
	bool autoIndex_;
	std::vector<std::string> indexList_;
	size_t connectionMaxBodySize_;
	std::set<t_method> methods_;
	t_return return_;
	std::set<t_error_page> errorPages_;
	std::vector<ConfigBlock> children;

	ConfigBlock();
};

ConfigBlock::ConfigBlock()
{
	type = CONTEXT_BLOCK;
	connectionMaxBodySize_ = 0;
}

std::ostream &operator<<(std::ostream &stream, const ConfigBlock &block)
{
	stream << "- PRINTING BLOCK (" << block.type << ") -\nPorts: {";
	for (std::set<int>::const_iterator portsIt = block.ports_.begin(); portsIt != block.ports_.end(); ++portsIt)
		stream << *portsIt << ", ";
	stream << "}\nNames: {";
	for (size_t i = 0; i < block.names_.size(); i++)
		stream << block.names_.at(i) << ", ";
	stream << "}\nUpload path: " << block.uploadPath_ << std::endl
		   << "Path: " << block.path_ << std::endl
		   << "Root: " << block.root_ << std::endl
		   << "Cgis: {";
	for (std::map<std::string, std::string>::const_iterator it = block.cgis_.begin(); it != block.cgis_.end(); ++it)
		stream << it->first << " => " << it->second << ", ";
	stream << "}\nAuto index: " << block.autoIndex_ << std::endl
		   << "Index list: {";
	for (std::vector<std::string>::const_iterator it = block.indexList_.begin(); it != block.indexList_.end(); ++it)
		stream << *it << ", ";
	stream << "}\nMax body size: " << block.connectionMaxBodySize_ << std::endl
		   << "Methods: {";
	for (std::set<t_method>::const_iterator it = block.methods_.begin(); it != block.methods_.end(); ++it)
		stream << (*it) << ", ";
	stream << "}\nReturn: " << block.return_.code << " " << block.return_.path << std::endl
		   << "Error pages: {";
	for (std::set<t_error_page>::const_iterator it = block.errorPages_.begin(); it != block.errorPages_.end(); ++it)
		stream << it->code << " " << it->page << ", ";
	stream << "}" << std::endl
		   << std::endl;

	return stream;
}
