#include "Location.hpp"

void Location::setPath(const std::string &path)
{
	this->path_ = path;
}

void Location::setRoot(const std::string &root)
{
	this->root_ = root;
}

void Location::setAutoIndex(bool mode)
{
	this->autoIndex_ = mode;
}

void Location::setIndexList(const std::vector<std::string> indexes)
{
	this->indexList_ = indexes;
}

void Location::setMethods(const std::set<t_method> &methods)
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

std::set<t_method> Location::getMethods()
{
	return this->methods_;
}

std::set<t_error_page> Location::getErrorPages()
{
    return this->errorPages_;
}

bool Location::getAutoIndex()
{
	return this->autoIndex_;
}

std::vector<std::string> Location::getIndexList()
{
	return this->indexList_;
}
