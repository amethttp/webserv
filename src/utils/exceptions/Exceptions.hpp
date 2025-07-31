#pragma once
#include <exception>

class FatalException : public std::exception
{
	public:
		virtual const char *what(char *msg) const throw();
};

class ConnectionException : public std::exception
{
	public:
		virtual const char *what(char *msg) const throw();
};