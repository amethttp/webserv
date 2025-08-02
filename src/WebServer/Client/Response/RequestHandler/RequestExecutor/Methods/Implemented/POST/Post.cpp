#include "Post.hpp"

mPost::mPost()
{
}

static t_httpCode postFile(Context &ctx, t_HandlingResult res)
{
	if (pathExists(ctx.targetPath_))
		return CONFLICT;

	// normalizeTrailingSlash(ctx.targetPath_);
	std::ofstream file(ctx.targetPath_.c_str(), std::ofstream::trunc);
	if (!file.is_open())
	{
		file.close();
		throw (std::runtime_error("Error creating file"));
	}
	file << ctx.request_.body.getMessage();
	res.tempHeaders_.addHeader("Content-Location", ctx.targetPath_);

	return CREATED;
}

t_HandlingResult mPost::execute(Context &ctx)
{
	int statCheck;
	t_HandlingResult res;

	res.mode_ = ctx.getConnectionMode();
	statCheck = checkPath(ctx.uploadPath_);
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
