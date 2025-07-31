#include "Session.hpp"

Session::Session()
{
	this->id_ = 0;
}

Session::Session(int id)
{
	this->id_ = id;
}

int Session::getId()
{
	return this->id_;
}

void Session::setId(int id)
{
	this->id_ = id;
}

std::string Session::getDesc()
{
	return this->desc_;
}

void Session::setDesc(std::string desc)
{
	this->desc_ = desc;
}

Session::~Session()
{
}