
static bool checkReturn(Location &location)
{
	return (location.getReturn().code != 0);
}

void handleReturnDirective(Context &ctx, HandlingResult &res)
{
    if (!ret.path.empty())
	{
		if (isRedirection(ret.code))
			this->setRedirectionResponse(ret.code, ret.path);
		else
			this->setBodyFromString(ret.path);
	}
}

void handleExecution(Context &ctx, HandlingResult &res)
{
    res = RequestExecutor::execute(ctx);
}

void tryCustomErrorPage(Context &ctx, HandlingResult &res)
{
    t_errorPage errPage;

    if (matchCustomErrorPage(code, ctx.location_, errorPage))
	{
		// config parse that an error page MUST have a URI page 
		this->setBodyFromFile(errorPage.page);
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