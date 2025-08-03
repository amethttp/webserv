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

t_HandlingResult mDelete::execute(const Context &ctx)
{
	t_HandlingResult res;
	int statCheck;

	res.mode_ = ctx.getConnectionMode();
	statCheck = checkPath(ctx.getTargetPath());
	switch (statCheck)
	{
		case S_IFREG:
			res.code_ = removeFile(ctx.getTargetPath().c_str());
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
