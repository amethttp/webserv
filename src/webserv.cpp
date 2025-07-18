#include <errno.h>
#include <string.h>
#include <iostream>
#include "WebServer/WebServer.hpp"

static void setLocationsPH(Server &server)
{
	Location *location;
	std::vector<Location *> testLocations;
	std::vector<std::string> indexes;
	std::set<t_method> allowedMethods;
	t_return ret;

	std::map<std::string, std::string> test;

	indexes.push_back("index");
	indexes.push_back("index.html");
	indexes.push_back("test.html");
	indexes.push_back("test2.html");
	allowedMethods.insert(M_GET);
	allowedMethods.insert(M_POST);
	test[".py"] = "/usr/bin/python3";
	test[".sh"] = "/bin/bash";

	location = new Location();
	location->setCGIs(test);
	location->setRoot("tests/www");
	location->setPath("/");
	location->setAutoIndex(true);
	location->setIndexList(indexes);
	location->setMethods(allowedMethods);
	testLocations.push_back(location);

	location = new Location();
	location->setRoot("tests/www");
	location->setPath("/test/index");
	ret.code = MOVED_PERMANENTLY;
	ret.path = "/test/index/";
	location->setReturn(ret);
	location->setAutoIndex(false);
	location->setIndexList(indexes);
	location->setMethods(allowedMethods);
	testLocations.push_back(location);
	server.setLocations(testLocations);
	server.setUploadPath("tests/www/uploads/");
}

int main(void)
{
    try
    {
        WebServer webServer = WebServer();

        std::vector<Server *> servers;
        std::vector<int> ports;
        Server *server;

        ports.push_back(8080);
        ports.push_back(9000);
        server = new Server();
        server->setPorts(ports);
        setLocationsPH(*server);
        servers.push_back(server);

        ports.clear();
        ports.push_back(3004);
        server = new Server();
        server->setPorts(ports);
        servers.push_back(server);

        ports.clear();
        ports.push_back(4444);
        ports.push_back(5555);
        ports.push_back(6000);
        server = new Server();
        server->setPorts(ports);
        servers.push_back(server);

        ports.clear();
        ports.push_back(3000);
        ports.push_back(3241);
        server = new Server();
        server->setPorts(ports);
        servers.push_back(server);

        webServer.setServers(servers);
        webServer.serve();
    }
    catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what()
            << ": " << strerror(errno) << std::endl;
    }

    return 0;
}
