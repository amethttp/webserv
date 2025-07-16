#pragma once

#include <sstream>
#include <stdlib.h>
#include "utils/cgi/cgi.hpp"
#include "utils/fileHandler/files.hpp"
#include "WebServer/Client/RequestHandler/RequestExecutor/Methods/AMethod.hpp"

#define INDEX_STYLE "src/utils/htmlTemplates/indexStyle.html"
#define INDEX_FILE_LIST "src/utils/htmlTemplates/indexFileList.html"
#define INDEX_CLOSE "src/utils/htmlTemplates/closeIndex.html"

class mGet : public AMethod
{
private:
public:
	mGet();
	~mGet();

	HandlingResult execute(Context &ctx);
};
