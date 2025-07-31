#pragma once
#include <string>
class Session
{
private:
	int id_;
	std::string desc_;

public:
	Session();
	Session(int id);
	int getId();
	void setId(int);
	std::string getDesc();
	void setDesc(std::string);
	~Session();
};
