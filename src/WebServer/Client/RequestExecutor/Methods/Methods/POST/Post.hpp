#include "Methods/AMethod.hpp"
#include "utils/fileHandler/files.hpp"

class mPost : public AMethod
{
private:
public:
	mPost();
	~mPost();

	ExecutionResult execute(Context &ctx);
};