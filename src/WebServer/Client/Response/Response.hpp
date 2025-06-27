#pragma once

#include <string>
#include "utils/http.hpp"
#include "StatusLine/StatusLine.hpp"
#include "Parameters/Parameters.hpp"
#include "WebServer/Server/Server.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Server/Location/Location.hpp"

typedef struct s_body
{
	std::string content;
	std::string type;
} t_body;

class Response
{
private:
	static std::map<t_httpCode, std::string> errorDict_; // to client
	static std::map<std::string, std::string> extensionTypesDict_; // to client

	std::string buffer_; // to client

	StatusLine statusLine_;
	std::map<std::string, std::string> headers_; // make response a dto/simpler class
	t_body body_;

	// to response generator
	void setStatusLine(t_httpCode code);
	void setResponseHeaders(t_connection mode);
	void setRepresentationHeaders();
	
	void generateResponse(t_httpCode code, t_connection mode);

	// request executor
	std::string getMIME(std::string &target);
	t_httpCode tryAutoIndex(Parameters &p);
	t_httpCode getFile(std::string &target);
	t_httpCode methodGet(Parameters &p);
	t_httpCode methodPost(Parameters &p);
	t_httpCode methodDelete(Parameters &p);
	t_httpCode executeRequest(Parameters &p);
public:
	Response();
	~Response();
	std::string toString();

	void setBuffer(const std::string &buffer);
	void eraseBuffer(size_t bytesToErase);

	std::string getBuffer() const;
	t_httpCode getStatusCode() const;
	bool getConnection() const;

	void clear();

	void build(Parameters &responseParams);
	void build(t_httpCode code, t_connection mode);
};
