#include <vector>
#include "Methods/AMethod.hpp"
#include "Methods/GET/Get.hpp"
#include "Methods/POST/Post.hpp"
#include "Methods/DELETE/Delete.hpp"

typedef AMethod* (*CreateFunc)();

class MethodFactory
{
private:
	std::map<t_method, CreateFunc>	registry_;
public:
    ~MethodFactory();
	MethodFactory();
	MethodFactory(std::vector<t_method> &methods);

    bool addMethod(const t_method& type, CreateFunc func);
    AMethod *create(const t_method& type);
};

AMethod *createMethodGet() { return new mGet(); };
AMethod *createMethodPost() { return new mPost(); };
AMethod *createMethodDelete() { return new mDelete(); };