#include "Delete.hpp"

mDelete::mDelete()
{
}

static t_httpCode removeFile(const char *path)
{
	if (std::remove(path) < 0)
		throw std::runtime_error("Couldn't remove resource");

	return NO_CONTENT;
}

HandlingResult mDelete::execute(Context &ctx)
{
	HandlingResult res;

	int statCheck;

	statCheck = checkPath(ctx.targetPath_);
	switch (statCheck)
	{
		case S_IFREG:
			res.code_ = removeFile(ctx.targetPath_.c_str());
			break ;
		case EACCES:
		case S_IFDIR:
			res.code_ = FORBIDDEN;
			break ;

		default:
			res.code_ = NOT_FOUND;
			break ;
	}

	return res;
}

mDelete::~mDelete()
{
}