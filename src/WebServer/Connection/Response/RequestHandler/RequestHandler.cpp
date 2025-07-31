#include "RequestHandler.hpp"
#include <string>
#include <ctime>
#include <sstream>
#include "WebServer/Connection/Connection.hpp"
#include "utils/CookieHelper/CookieHelper.hpp"
#include "utils/string/string.hpp"

static bool isRedirection(t_httpCode code)
{
	return ((code / 100) * 100 == 300);
}

static std::string getRedirectionHTML(t_httpCode code, std::string &uri)
{
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		<<"<html>\n" 
		<< "<head><title>" << code << " " << Connection::getHttpErrorMsg(code) << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>" << Connection::getHttpErrorMsg(code) << "</h1>\n"
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
	res.tempBody_.type = Connection::getExtensionType(".html");
}

static void setBodyFromString(std::string str, HandlingResult &res)
{
	res.tempBody_.content = str;
	res.tempBody_.type = Connection::getExtensionType(".txt");
}

void RequestHandler::handleReturnDirective(Context &ctx, HandlingResult &res)
{
	t_httpCode retCode = ctx.getReturn().code;
	std::string retPath = ctx.getReturn().path;

    if (!retPath.empty())
	{
		if (isRedirection(retCode))
			setRedirectionResult(retCode, retPath, res);
		else
			setBodyFromString(retPath, res);
	}

	res.code_ = retCode;
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
        res.tempBody_.content = readFileToString(errPage.page);
        res.tempBody_.type = getMIME(errPage.page);
	}
}

static void useSession(t_Request &request, Server &server, HandlingResult &result)
{
	std::cout << "Sessions amount: " << server.getSessions().size() << std::endl;
	std::string value;
	try
	{
		std::vector<std::string> values = split(request.headers.getHeaderValue(COOKIE_REQUEST_KEY), ";");
		for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end() && value.empty(); ++it)
		{
			if ((*it).find("sessionId=") != std::string::npos)
				value = *it;
		}
		if (value.empty())
			throw std::runtime_error("dummy throw");
		value = split(value, "=")[1];
		std::stringstream ss(value);
		int sessId;
		ss >> sessId;
		Session * sess = server.getSession(sessId);
		std::cout << "Session id: " << sess->getId() << ", desc: " << sess->getDesc() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::stringstream cookieVal;
		Session *session = new Session();
		server.pushSession(session);
		cookieVal << "sessionId=" << session->getId() << "; expires=Fri, 31 Dec 9999 23:59:59 GMT; HttpOnly";
		result.tempHeaders_.addHeader("Set-Cookie", cookieVal.str());
		std::cout << "------ NEW SESSION: " << session->getId() << std::endl;
	}
}

HandlingResult RequestHandler::handleRequest(t_Request &request, Location &location, Server &server)
{
    HandlingResult result;
    Context ctx(request, location, server);

	if (checkReturn(ctx.location_))
		handleReturnDirective(ctx, result);
	else
		handleExecution(ctx, result);
    tryCustomErrorPage(ctx, result);
	useSession(request, server, result);

    return result;
}
