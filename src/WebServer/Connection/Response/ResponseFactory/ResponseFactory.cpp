#include <sstream>
#include "ResponseFactory.hpp"
#include "WebServer/Connection/Connection.hpp"
#include "utils/exceptions/Exceptions.hpp"

static void setResponseBuffer(t_Response &response)
{
	std::ostringstream res;

	res << response.statusLine_;
	res << response.headers_;
	res << response.body_.content;

	response.buffer_ = res.str();
}

void ResponseFactory::addResultHeaders(HeaderCollection &resultHeaders, t_Response &r)
{
	const std::vector<Header> &resHeaders = resultHeaders.getHeaders();

	for (size_t i = 0; i < resHeaders.size(); ++i)
		r.headers_.addHeader(resHeaders[i].getKey(), resHeaders[i].getValue());
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
	response.headers_.addHeader(CONNECTION, (mode ? "close" : "keep-alive"));
}

static void setRepresentationHeaders(t_Response &response)
{
	std::ostringstream length;

	length << response.body_.content.length();
	response.headers_.addHeader(CONTENT_LENGTH, length.str());
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
	response.body_.type = body.type;
	response.body_.content = body.content;
}

void ResponseFactory::setStatusLine(t_httpCode code, t_Response &response)
{
	std::string codeMsg = Connection::getHttpErrorMsg(code);

	if (codeMsg.empty())
	{
		int defaultCode = (code / 100) * 100;
		if (defaultCode != 100 &&
			defaultCode != 200 &&
			defaultCode != 300 &&
			defaultCode != 400 &&
			defaultCode != 500)
		{
			throw RecoverableException("Couldn't determine error code");
		}
		code = static_cast<t_httpCode>(defaultCode);
		codeMsg = Connection::getHttpErrorMsg(code);
	}

	response.statusLine_.setFields(code, codeMsg);
}

t_Response ResponseFactory::create(t_HandlingResult &res)
{
	t_Response response;

	setStatusLine(res.code_, response);
	setResponseBody(res.tempBody_, response);
	setDefaultHeaders(res.mode_, response);
	addResultHeaders(res.tempHeaders_, response);
	if (res.isCGI_)
		response.buffer_ = res.tempBody_.content;
	else
		setResponseBuffer(response);

    return response;
}

t_Response ResponseFactory::create(t_httpCode code, t_connection mode)
{
	t_Response response;

	setStatusLine(code, response);
	setDefaultHeaders(mode, response);
	setResponseBuffer(response);

    return response;
}
