#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "utils/epoll.hpp"
#include "Client/Client.hpp"
#include "Server/Server.hpp"
#include "utils/Result/Result.hpp"

#define READ_BUFFER_SIZE 4096
#define TIMEOUT 5000

class WebServer
{
private:
	std::vector<Server *> servers_;
	std::vector<Client *> clients_;

	std::vector<fd_t> createServerFds();

	void setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds);
	void setEpollRead(t_epoll &epoll, Client *client);
	void setEpollWrite(t_epoll &epoll, Client *client);

	fd_t getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd);

	void acceptNewClient(fd_t &serverFd, t_epoll &epoll);
	std::vector<Client *>::iterator disconnectClient(Client *client, t_epoll &epoll, const std::string &reason);
	std::vector<Client *>::iterator removeClient(Client *client);
	void checkClientEvent(t_epoll &epoll, const int &eventIndex);
	void handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll);
	void disconnectTimedoutClients(t_epoll &epoll);

	void receiveRequest(Client *client, t_epoll &epoll);

	Server *matchServer(std::string hostName);
	void processRequest(Client *client, Result<t_Request> &result);
	void sendResponse(Client *client, t_epoll &epoll);

public:
	WebServer();
	~WebServer();

	void setServers(std::vector<Server *> &servers);

	void serve();

	friend std::ostream &operator<<(std::ostream &stream, const WebServer &ws);
};
