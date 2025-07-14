#include "ResponseFactory.hpp"

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

void ResponseFactory::setStatusLine(t_httpCode code)
{
	if (this->errorDict_.find(code) == this->errorDict_.end())
	{
		int defaultCode = (code / 100) * 100;
		if (defaultCode != 100 ||
			defaultCode != 200 ||
			defaultCode != 300 ||
			defaultCode != 400 ||
			defaultCode != 500)
		{
			throw std::runtime_error("Couldn't determine error code");			
		}
	}
	this->statusLine_.setFields(code, this->errorDict_[code]);
}

void ResponseFactory::setResponseHeaders(t_connection mode)
{
	this->headers_["Server"] = "Amethttp";
	this->headers_["Date"] = getImfFixdate();
	this->headers_["Connection"] = mode ? "close" : "keep-alive";
}

void ResponseFactory::setRepresentationHeaders()
{
	std::ostringstream length;

	length << this->body_.content.length();
	this->headers_["Content-Length"] = length.str();
	if (!this->body_.content.empty())
		this->headers_["Content-Type"] = this->body_.type;
}

Response ResponseFactory::generateResponse(t_httpCode code, t_connection mode)
{
	Response res;

	setStatusLine(code);
	this->setResponseHeaders(mode);
	this->setRepresentationHeaders();

	return res;
}

Response ResponseFactory::create(HandlingResult &res)
{
	// body and stuff
    return generateResponse(res.code, res.mode);
}

Response ResponseFactory::create(t_httpCode code, t_connection mode)
{
    return generateResponse(code, mode);
}