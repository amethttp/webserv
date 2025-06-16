#pragma once

#include <string>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "Parameters/Parameters.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Location/Location.hpp"

class Response
{
private:
	static std::map<t_httpCode, std::string> errorDict_;
	static std::map<std::string, std::string> extensionTypesDict_;

	std::string buffer_;

	StatusLine statusLine_;
	std::map<std::string, std::string> headers_;
	std::string body_;

	std::string getMIME(std::string &target);
	void setStatusLine(t_httpCode code);
	void setResponseHeaders(t_connection mode);
	void setRepresentationHeaders(std::string &target);
	
	void generateResponse(t_httpCode code, t_connection mode);

	t_httpCode methodGet(std::string targetPath);
	t_httpCode methodPost(std::string targetPath);
	t_httpCode methodDelete(std::string targetPath);
	t_httpCode executeRequest(const Parameters &p);
public:
	Response();
	~Response();
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(size_t bytesToErase);

	std::string getBuffer() const;
	std::string getBody() const;
	t_httpCode getStatusCode() const;
	bool getConnection() const;

	void build(const Parameters &responseParams);
	void build(t_httpCode code, t_connection mode);
};
