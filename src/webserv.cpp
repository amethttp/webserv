#include <errno.h>
#include <string.h>
#include <iostream>
#include "WebServer/WebServer.hpp"

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
