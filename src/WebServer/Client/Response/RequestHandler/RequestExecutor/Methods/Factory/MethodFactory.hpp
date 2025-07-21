#pragma once

#include <vector>
#include "../AMethod.hpp"
#include "../Implemented/GET/Get.hpp"
#include "../Implemented/POST/Post.hpp"
#include "../Implemented/DELETE/Delete.hpp"

typedef AMethod* (*CreateFunc)();

class MethodFactory
{
private:
	std::map<t_method, CreateFunc>	registry_;
public:
    ~MethodFactory();
	MethodFactory();
	MethodFactory(std::set<t_method> methods);

    bool addMethod(const t_method& type, CreateFunc func);
    AMethod *create(const t_method& type);
};

AMethod *createMethodGet();
AMethod *createMethodPost();
AMethod *createMethodDelete();
