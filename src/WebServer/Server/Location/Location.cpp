#include "Location.hpp"

void Location::setPath(const std::string &path)
{
	this->path_ = path;
}

void Location::setRoot(const std::string &root)
{
	this->root_ = root;
}

void Location::setMethods(const std::set<method_t> &methods)
{
	this->methods_ = methods;
}

std::string Location::getPath()
{
	return this->path_;
}

std::string Location::getRoot()
{
	return this->root_;
}

std::set<method_t> Location::getMethods()
{
	return this->methods_;
}
