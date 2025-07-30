#include <iostream>
#include "Location.hpp"

Location::Location()
{
	clientMaxBodySize_ = 0;
}

Location::~Location()
{
}

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

void Location::setReturn(const t_return &ret)
{
	this->return_.code = ret.code;
	if (!ret.path.empty())
		this->return_.path = ret.path;
}

void Location::setMaxBodySize(const size_t size)
{
	this->clientMaxBodySize_ = size;
}

void Location::setCGIs(std::map<std::string, std::string> cgis)
{
	this->cgis_ = cgis;
}

void Location::setErrorPages(const std::set<t_error_page> errorPages_)
{
	this->errorPages_ = errorPages_;
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

std::map<std::string, std::string> Location::getCGIs()
{
	return this->cgis_;
}

t_return Location::getReturn()
{
	return this->return_;
}

bool Location::getAutoIndex()
{
	return this->autoIndex_;
}

std::vector<std::string> Location::getIndexList()
{
	return this->indexList_;
}

std::ostream &operator<<(std::ostream &stream, const Location &location)
{
	stream << "- LOCATION -" << std::endl
		   << "Path: " << location.path_ << std::endl
		   << "Root: " << location.root_ << std::endl
		   << "Cgis: {";
	for (std::map<std::string, std::string>::const_iterator it = location.cgis_.begin(); it != location.cgis_.end(); ++it)
		stream << it->first << " => " << it->second << ", ";
	stream << "}\nAuto index: " << location.autoIndex_ << std::endl
		   << "Index list: {";
	for (std::vector<std::string>::const_iterator it = location.indexList_.begin(); it != location.indexList_.end(); ++it)
		stream << *it << ", ";
	stream << "}\nMax body size: " << location.clientMaxBodySize_ << std::endl
		   << "Methods: {";
	for (std::set<t_method>::const_iterator it = location.methods_.begin(); it != location.methods_.end(); ++it)
		stream << (*it) << ", ";
	stream << "}\nReturn: " << location.return_.code << " " << location.return_.path << std::endl
		   << "Error pages: {";
	for (std::set<t_error_page>::const_iterator it = location.errorPages_.begin(); it != location.errorPages_.end(); ++it)
		stream << it->code << " " << it->page << ", ";
	stream << "}" << std::endl;
	return stream;
}

void Location::setDefaults()
{
	if (path_.empty())
		path_ = "/";
	if (root_.empty())
		root_ = "html";
	if (!autoIndex_)
		autoIndex_ = false;
	if (indexList_.empty())
		indexList_.push_back("index.html");
	if (clientMaxBodySize_ == 0)
		clientMaxBodySize_ = 1048576;
	if (methods_.empty())
	{
		methods_.insert(M_GET);
		methods_.insert(M_POST);
	}
}
