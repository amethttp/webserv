#include "RequestHandler.hpp"
#include <string>
#include <ctime>
#include <sstream>
#include "WebServer/Client/Client.hpp"

static bool isRedirection(t_httpCode code)
{
	return ((code / 100) * 100 == 300);
}

static std::string getRedirectionHTML(t_httpCode code, std::string &uri)
{
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		<<"<html>\n" 
		<< "<head><title>" << code << " " << Client::getHttpErrorMsg(code) << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << Client::getHttpErrorMsg(code) << "</h1>\n"
		<< "<p>The document has moved <a href=\"" << uri << "\">" << "here" << "</a>.</p>\n"
		<< "</body>\n"
		<< "</html>\n";
	
	return html.str();
}

static bool checkReturn(Location &location)
{
	return (location.getReturn().code != 0);
}

static void setRedirectionResult(t_httpCode code, std::string uri, HandlingResult &res)
{
	res.tempHeaders_.addHeader("Location", uri);
	res.tempBody_.content = getRedirectionHTML(code, uri);
	res.tempBody_.type = Client::getExtensionType(".html");
}

static void setBodyFromString(std::string str, HandlingResult &res)
{
	res.tempBody_.content = str;
	res.tempBody_.type = Client::getExtensionType(".txt");
}

void RequestHandler::handleReturnDirective(Context &ctx, HandlingResult &res)
{
    t_return ret = ctx.getReturn();

    if (!ret.path.empty())
	{
		if (isRedirection(ret.code))
			setRedirectionResult(ret.code, ret.path, res);
		else
			setBodyFromString(ret.path, res);
	}
}

void RequestHandler::handleExecution(Context &ctx, HandlingResult &res)
{
    res = RequestExecutor::executeRequest(ctx);
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

static void tryCustomErrorPage(Context &ctx, HandlingResult &res)
{
    t_error_page errPage;

    if (matchCustomErrorPage(res.code_, ctx.location_, errPage))
	{
		// config parse that an error page MUST have a URI page 
        res.tempBody_.content =  readFileToString(errPage.page);
        res.tempBody_.type = getMIME(errPage.page);
	}
}

HandlingResult RequestHandler::handleRequest(Request &request, Location &location, Server &server)
{
    HandlingResult result;
    Context ctx(request, location, server);

	if (checkReturn(ctx.location_))
		handleReturnDirective(ctx, result);
	else
		handleExecution(ctx, result);
    
    tryCustomErrorPage(ctx, result);

    return result;
}
