#include "Parameters.hpp"

Parameters::Parameters(Request &req, Location &loc) : request_(req), location_(loc)
{
	this->setParameters();
}

void Parameters::checkRequestHeaders()
{
	std::map<std::string, std::string> reqHeaders = this->request_.getHeaders();

	if (reqHeaders.find("Connection") != reqHeaders.end())
	{
		if (reqHeaders["Connection"] == "close")
			this->connectionMode_ = C_CLOSE;
	}
}

void Parameters::routeTarget()
{
	this->targetPath_ = this->location_.getRoot() + this->request_.getTarget();
}

void  Parameters::fitMethod()
{
	t_method reqMethod = this->request_.getMethod();

	if (reqMethod == M_NOT_IMPLEMENTED)
		this->method_ = M_NOT_IMPLEMENTED;
	else
	{
		std::set<t_method> allowedMethods = this->location_.getMethods();

		if (allowedMethods.find(reqMethod) != allowedMethods.end())
			this->method_ = reqMethod;
		else
			this->method_ = M_NOT_ALLOWED;
	}
}

void Parameters::setParameters()
{
	this->connectionMode_ = C_KEEP_ALIVE;

	routeTarget();
	fitMethod();
	checkRequestHeaders();
}

std::string Parameters::getTargetPath() const
{
	return this->targetPath_;
}

t_connection Parameters::getConnectionMode() const
{
	return this->connectionMode_;
}

t_method Parameters::getMethod() const
{
	return this->method_;
}

Parameters::~Parameters()
{
}