#pragma once
#include <exception>
#include <string>

class FatalException : public std::exception
{
private:
	std::string exceptionMsg;
public:
	FatalException(const char *msg);
	virtual ~FatalException() throw();

	const char *what() const throw();
};

class RecoverableException : public std::exception
{
private:
	std::string exceptionMsg;
public:
	RecoverableException(const char *msg);
	virtual ~RecoverableException() throw();

	const char *what() const throw();
};
