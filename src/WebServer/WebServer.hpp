#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "utils/epoll.hpp"
#include "Client/Client.hpp"
#include "Server/Server.hpp"

#define READ_BUFFER_SIZE 4096

class WebServer
{
private:
	std::vector<Server> servers_;

	std::vector<fd_t> createServerFds();
	void setEpoll(t_epoll &epoll, std::vector<fd_t> &serversFds);
	fd_t getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd);
	void acceptNewClient(fd_t &serverFd, t_epoll &epoll);
	void disconnectClient(Client *client, t_epoll &epoll);
	void buildResponse(Client *client, t_epoll &epoll);
	void receiveRequest(Client *client, t_epoll &epoll);
	void sendResponse(Client *client, t_epoll &epoll);
	void checkClientEvent(t_epoll &epoll, const int &eventIndex);
	void handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll);

public:
	WebServer();
	~WebServer();

	std::vector<Server> getServers();
	void setServers(std::vector<Server> &servers);

	void serve();
};
