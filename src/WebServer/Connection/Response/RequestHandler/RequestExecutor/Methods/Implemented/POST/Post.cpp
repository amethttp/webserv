#include "Post.hpp"
#include "utils/exceptions/Exceptions.hpp"

mPost::mPost()
{
}

static t_httpCode postFile(const Context &ctx, t_HandlingResult &res)
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

static t_HandlingResult run(const Context &ctx, t_HandlingResult &res)
{
	int statCheck;

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

static t_httpCode handleCgiOutput(const Context &ctx, t_cgi &cgi, t_Body &body)
{
	int outPipe[2]; 
	int inPipe[2];  
	pid_t child;
	time_t startTime;

	if (pipe(outPipe) == -1)
		throw (RecoverableException("Ceci n'est pas une pipe"));
	if (pipe(inPipe) == -1)
	{
		close(outPipe[0]);
		close(outPipe[1]);
		throw (RecoverableException("Ceci n'est pas une pipe"));
	}
	startTime = std::time(NULL);
	child = fork();
	if (child < 0)
	{
		close(outPipe[0]);
		close(outPipe[1]);
		close(inPipe[0]);
		close(inPipe[1]);
		throw (RecoverableException("Couldn't fork CGI properly"));
	}
	else if (child == CHILD_OK)
	{
		char **env = setEnvironment(ctx, cgi);
		char **argv = setArgs(ctx, cgi);

		close(outPipe[0]);
		close(inPipe[1]);
		
		dup2(outPipe[1], STDOUT_FILENO);
		close(outPipe[1]);
		dup2(inPipe[0], STDIN_FILENO);
		close(inPipe[0]);

		execve(cgi.second.c_str(), argv, env);
		freeArr(env);
		freeArr(argv);
		exit(1);
	}
	close(outPipe[1]);  
	close(inPipe[0]);   
	
	sendBody(inPipe, body);
	close(inPipe[1]);

	return waitForOutput(child, outPipe, startTime, body);
}

static void runCGI(const Context &ctx, t_cgi &cgi, t_HandlingResult &res) // TODO: extract this into method...
{
	res.code_ = handleCgiOutput(ctx, cgi, res.tempBody_);
	if (res.code_ == OK)
		res.isCGI_ = true;
	else if (res.code_ == GATEWAY_TIME_OUT)
		res.mode_ = C_CLOSE;
}

t_HandlingResult mPost::execute(const Context &ctx)
{
	t_cgi cgi;
	t_HandlingResult res;

	res.mode_ = ctx.getConnectionMode();
	if (matchCGI(ctx.getTargetPath(), ctx.getLocation(), cgi))
		runCGI(ctx, cgi, res);
	else
		run(ctx, res);

	return res;
}

mPost::~mPost()
{
}
