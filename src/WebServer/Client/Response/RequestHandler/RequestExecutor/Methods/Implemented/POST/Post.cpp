#include "Post.hpp"
#include "utils/exceptions/Exceptions.hpp"

mPost::mPost()
{
}

static t_httpCode postFile(const Context &ctx, t_HandlingResult res)
{
	if (pathExists(ctx.getTargetPath()))
		return CONFLICT;

	// normalizeTrailingSlash(ctx.getTargetPath());
	std::ofstream file(ctx.getTargetPath().c_str(), std::ofstream::trunc);
	if (!file.is_open())
	{
		file.close();
		throw (RecoverableException("Error creating file"));
	}
	file << ctx.getRequest().body.getMessage();
	res.tempHeaders_.addHeader("Content-Location", ctx.getTargetPath());

	return CREATED;
}

t_HandlingResult mPost::execute(const Context &ctx)
{
	int statCheck;
	t_HandlingResult res;

	res.mode_ = ctx.getConnectionMode();
	statCheck = checkPath(ctx.getUploadPath());
	switch (statCheck)
	{
		case S_IFDIR:
			res.code_ = postFile(ctx, res);
			break ;
		case EACCES:
			res.code_ = FORBIDDEN;
			break ;

		default:
			res.code_ = NOT_FOUND;
			break ;
	}

	return res;
}

mPost::~mPost()
{
}
