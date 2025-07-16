#include "MethodFactory.hpp"

AMethod *createMethodGet() { return new mGet(); }
AMethod *createMethodPost() { return new mPost(); }
AMethod *createMethodDelete() { return new mDelete(); }

MethodFactory::MethodFactory()
{
}

MethodFactory::MethodFactory(std::set<t_method> methods)
{
	for (std::set<t_method>::iterator methodIt = methods.begin(); methodIt != methods.end(); ++methodIt)
	{
		switch (*methodIt)
		{
			case M_GET:
				addMethod(M_GET, createMethodGet);
				break;
			case M_POST:
				addMethod(M_POST, createMethodPost);
				break;
			case M_DELETE:
				addMethod(M_DELETE, createMethodDelete);
				break;
		}
	}
}

bool MethodFactory::addMethod(const t_method &type, CreateFunc func)
{
	return registry_.insert(std::make_pair(type, func)).second;
}

AMethod* MethodFactory::create(const t_method& type)
{
	std::map<t_method, CreateFunc>::iterator it = registry_.find(type);
	if (it != registry_.end()) {
		return it->second();
	}

	return NULL;
}

MethodFactory::~MethodFactory()
{
}
