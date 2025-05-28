#include <iostream>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "WebServer/WebServer.hpp"

int main(int argc, char *argv[])
{
	WebServer webserver;
	std::string configFile;
	if (argc > 1)
		configFile = argv[1];
	else
		configFile = "/default.conf";
	webserver.readConfigFile(configFile);
	webserver.serve();
	return 0;
}
