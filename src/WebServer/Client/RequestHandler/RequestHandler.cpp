
static bool checkReturn(Location &location)
{
	return (location.getReturn().code != 0);
}

void setRedirectionHeaders(t_httpCode code, std::string uri, HandlingResult &res)
{
	res.tempHeaders_["Location"] = uri;
	res.tempBody_.content = getRedirectionHTML(code, uri);
	res.tempBody_.type = this->extensionTypesDict_[".html"];
}

void setBodyFromString(std::string str, HandlingResult &res)
{
	res.tempBody_.content = str;
	res.tempBody_.type = this->extensionTypesDict_[".txt"];
}

void handleReturnDirective(Context &ctx, HandlingResult &res)
{
    t_return ret = ctx.getReturn();

    if (!ret.path.empty())
	{
		if (isRedirection(ret.code))
			setRedirectionHeaders(ret.code, ret.path, res);
		else
			setBodyFromString(ret.path, res);
	}
}

void handleExecution(Context &ctx, HandlingResult &res)
{
    res = RequestExecutor::execute(ctx);
}

static bool matchCustomErrorPage(t_httpCode code, Location &location, t_error_page &page)
{
	std::set<t_error_page> errorPages = location.getErrorPages();
	bzero(&page, sizeof(page));

	for (std::set<t_error_page>::iterator ite = errorPages.begin(); ite != errorPages.end(); ++ite)
	{
		if (ite->code == code)
		{
			page = *ite;
			return true;
		}
	}

	return false;
}

void tryCustomErrorPage(Context &ctx, HandlingResult &res)
{
    t_errorPage errPage;

    if (matchCustomErrorPage(code, ctx.location_, errorPage))
	{
		// config parse that an error page MUST have a URI page 
        res.tempBody_.content =  readFileToString(path.c_str());
        res.tempBody_.type = getMIME(path);
	}
}

HandlingResult handleRequest(Request &request, Location &location, Server &server)
{
    HandlingResult res;
    Context ctx(request, location, server);

	if (checkReturn(ctx.location_))
		handleReturnDirective(ctx, res);
	else
		handleExecution(ctx, res);
    
    tryCustomErrorPage(ctx, res);

    return res;
}
