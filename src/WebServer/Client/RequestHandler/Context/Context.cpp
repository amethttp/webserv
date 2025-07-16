#include "Context.hpp"

Context::Context(Request &req, Location &loc, Server &server) : request_(req), location_(loc)
{
	this->connectionMode_ = C_KEEP_ALIVE;

	routeTarget();
	fitMethod();
	checkRequestHeaders();

	this->uploadPath_ = server.getUploadPath();
}

void Context::checkRequestHeaders()
{
	std::map<std::string, std::string> reqHeaders = this->request_.getHeaders();

	if (reqHeaders.find("Connection") != reqHeaders.end())
	{
		if (reqHeaders["Connection"] == "close")
			this->connectionMode_ = C_CLOSE;
	}
}

void Context::routeTarget()
{
	this->targetPath_ = this->location_.getRoot() + this->request_.getTarget();
}

void  Context::fitMethod()
{
	t_method reqMethod = this->request_.getMethod();
	std::set<t_method> allowedMethods = this->location_.getMethods();

	if (allowedMethods.find(reqMethod) != allowedMethods.end())
		this->method_ = reqMethod;
	else
		this->method_ = M_NOT_ALLOWED;
}

std::string Context::getTargetPath() const
{
	return this->targetPath_;
}

t_connection Context::getConnectionMode() const
{
	return this->connectionMode_;
}

t_return Context::getReturn() const
{
	return this->location_.getReturn();
}

t_method Context::getMethod() const
{
	return this->method_;
}

Context::~Context()
{
}
