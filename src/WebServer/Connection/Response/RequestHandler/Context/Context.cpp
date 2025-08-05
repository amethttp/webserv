#include "Context.hpp"
#include "WebServer/WebServer.hpp"
#include "utils/string/string.hpp"
#include "utils/exceptions/Exceptions.hpp"

Context::Context() : request_(NULL), location_(NULL), server_(NULL)
{
	this->connectionMode_ = C_KEEP_ALIVE;
}

Context::Context(const t_Request *req, const Location *loc, Server *server) : request_(req), location_(loc), server_(server)
{
	this->connectionMode_ = C_KEEP_ALIVE;

	routeTarget();
	fitMethod();
	checkRequestHeaders();

	this->uploadPath_ = this->server_->getUploadPath();
}

void Context::init(const std::vector<Server *> &servers, const t_Request &request)
{
	this->request_ = &request;
	this->server_ = WebServer::matchServer(servers, this->request_->headers.getHeaderValue(HOST));
	if (!this->server_)
		throw RecoverableException("Couldn't match server");
	this->location_ = this->server_->matchLocation(this->request_->requestLine.getTargetPath());

	routeTarget();
	fitMethod();
	checkRequestHeaders();

	this->uploadPath_ = this->server_->getUploadPath();
}

void Context::checkRequestHeaders()
{
	if (this->request_->headers.contains(CONNECTION))
	{
		if (this->request_->headers.getHeaderValue(CONNECTION) == "close")
			this->connectionMode_ = C_CLOSE;
	}
}

void Context::routeTarget()
{
	this->targetPath_ = this->location_->getRoot() + this->request_->requestLine.getTargetPath();
	removeDoubleSlashes(this->targetPath_);
}

void  Context::fitMethod()
{
	t_method reqMethod = this->request_->requestLine.getMethod();
	std::set<t_method> allowedMethods = this->location_->getMethods();

	if (allowedMethods.find(reqMethod) != allowedMethods.end())
		this->method_ = reqMethod;
	else
		this->method_ = M_NOT_ALLOWED;
}

void Context::pushServerSession(Session *session)
{
	this->server_->pushSession(session);
}

std::string Context::getTargetPath() const
{
	return this->targetPath_;
}

std::string Context::getUploadPath() const
{
    return this->uploadPath_;
}

t_connection Context::getConnectionMode() const
{
	return this->connectionMode_;
}

void Context::setTargetPath(const std::string &path)
{
	this->targetPath_ = path;
}

t_return Context::getReturn() const
{
	return this->location_->getReturn();
}

t_method Context::getMethod() const
{
	return this->method_;
}

const t_Request &Context::getRequest() const
{
	return *this->request_;
}

const Location &Context::getLocation() const
{
	return *this->location_;
}

const Server &Context::getServer() const
{
    return *this->server_;
}

Context::~Context()
{
}
