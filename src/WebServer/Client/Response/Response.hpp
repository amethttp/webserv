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
	static std::map<std::string, std::string> extensionTypesDict_;

	std::string buffer_;

	StatusLine statusLine;
	std::map<std::string, std::string> headers_;
	std::string body_;

	void setResponseHeaders();
	void setRepresentationHeaders(std::string &target);

	std::string getMIME(std::string &target);

	bool methodGet(std::string &path);
	bool methodPost(std::string &path);
	bool methodDelete(std::string &path);
public:
	std::string toString();

	void setBuffer(const std::string &buffer);

	std::string getBuffer();
	std::string getBody();

	void eraseBuffer(size_t bytesToErase);

	bool tryBuild(Request &request);
};
