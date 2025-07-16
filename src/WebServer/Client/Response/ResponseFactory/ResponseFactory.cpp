#include "ResponseFactory.hpp"
#include <sstream>
#include "WebServer/Client/Client.hpp"

void ResponseFactory::addResultHeaders(HeaderCollection &resultHeaders, HeaderCollection &responseHeaders)
{
	responseHeaders = resultHeaders;
}

static std::string getImfFixdate()
{
    std::time_t timeNow = std::time(NULL);
    std::tm *gmt = std::gmtime(&timeNow);
    std::string res;
	size_t resSize;
	
	res.resize(50);
    if ((resSize = std::strftime(&res[0], res.size(), "%a, %d %b %Y %H:%M:%S GMT", gmt)))
	{
		res.resize(resSize);
        return res;
	}
	return std::string("");
}

static void setResponseHeaders(t_connection mode, t_Response &response)
{
	response.headers_.addHeader("Server", "Amethttp");
	response.headers_.addHeader("Date", getImfFixdate());
	response.headers_.addHeader("Connection", (mode ? "close" : "keep-alive"));
}

static void setRepresentationHeaders(t_Response &response)
{
	std::ostringstream length;

	length << response.body_.content.length();
	response.headers_.addHeader("Content-Length", length.str());
	if (!response.body_.content.empty())
		response.headers_.addHeader("Content-Type", response.body_.type);
}

void ResponseFactory::setDefaultHeaders(t_connection mode, t_Response &response)
{
	setResponseHeaders(mode, response);
	setRepresentationHeaders(response);
}

void ResponseFactory::setResponseBody(t_Body &body, t_Response &response)
{
	response.body_ = body;
}

void ResponseFactory::setStatusLine(t_httpCode code, t_Response &response)
{
	std::string codeMsg = Client::getHttpErrorMsg(code);

	if (codeMsg.empty())
	{
		int defaultCode = (code / 100) * 100;
		if (defaultCode != 100 &&
			defaultCode != 200 &&
			defaultCode != 300 &&
			defaultCode != 400 &&
			defaultCode != 500)
		{
			throw std::runtime_error("Couldn't determine error code");			
		}
		code = static_cast<t_httpCode>(defaultCode);
		codeMsg = Client::getHttpErrorMsg(code);
	}

	response.statusLine_.setFields(code, codeMsg);
}

t_Response ResponseFactory::create(HandlingResult &res)
{
	// body and stuff
	t_Response response;
	bzero(&response, sizeof(response));

	setStatusLine(res.code_, response); // handling res private/public...
	setResponseBody(res.tempBody_, response);
	setDefaultHeaders(res.mode_, response);
	addResultHeaders(res.tempHeaders_, response.headers_);

    return response;
}

t_Response ResponseFactory::create(t_httpCode code, t_connection mode)
{
	t_Response response;
	bzero(&response, sizeof(response));

	setStatusLine(code, response);
	setDefaultHeaders(mode, response);

    return response;
}
