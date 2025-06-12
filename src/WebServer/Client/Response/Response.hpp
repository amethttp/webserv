#pragma once

#include <string>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "../Request/Request.hpp"

#include "../../Server/Server.hpp"
#include "../../Server/Location/Location.hpp"

class Response
{
private:
	static std::map<int, std::string> errorDict_;

	std::string buffer_;

	StatusLine statusLine;
	std::map<std::string, std::string> headers_;
	std::string body_;

	void setResponseHeaders();
	void setRepresentationHeaders();

	bool methodGet(Request &request, Server &server, Location &location);
	bool methodPost(Request &request, Server &server, Location &location);
	bool methodDelete(Request &request, Server &server, Location &location);
public:
	std::string toString();

	void setBuffer(const std::string &buffer);
	void setBody(const std::string &body);

	std::string getBuffer();
	std::string getBody();

	void eraseBuffer(size_t bytesToErase);

	bool tryBuild(Request &request);
};
