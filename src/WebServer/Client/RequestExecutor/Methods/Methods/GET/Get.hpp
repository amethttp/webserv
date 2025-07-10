#include <sstream>
#include "utils/cgi/cgi.hpp"
#include "Methods/AMethod.hpp"
#include "utils/fileHandler/files.hpp"

#define INDEX_STYLE "src/utils/htmlTemplates/indexStyle.html"
#define INDEX_FILE_LIST "src/utils/htmlTemplates/indexFileList.html"
#define INDEX_CLOSE "src/utils/htmlTemplates/closeIndex.html"

class mGet : public AMethod
{
private:
public:
	mGet();
	~mGet();

	ExecutionResult execute(Context &ctx);
};