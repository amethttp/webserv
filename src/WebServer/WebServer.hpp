#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "utils/epoll.hpp"
#include "Connection/Connection.hpp"
#include "Server/Server.hpp"

#define READ_BUFFER_SIZE 4096
#define TIMEOUT 5000

class WebServer
{
private:
	std::vector<Server *> servers_;
	std::vector<Connection *> clients_;

	std::vector<fd_t> createServerFds();

	void setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds);
	void setEpollRead(t_epoll &epoll, Connection *client);
	void setEpollWrite(t_epoll &epoll, Connection *client);

	fd_t getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd);

	void acceptNewConnection(fd_t &serverFd, t_epoll &epoll);
	std::vector<Connection *>::iterator disconnectConnection(Connection *client, t_epoll &epoll, const std::string &reason);
	std::vector<Connection *>::iterator removeConnection(Connection *client);
	void checkConnectionEvent(t_epoll &epoll, const int &eventIndex);
	void handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll);
	void disconnectTimedoutConnections(t_epoll &epoll);

	void receiveRequest(Connection *client, t_epoll &epoll);
	bool tryBuildRequest(Connection *client, const char *buffer);

	Server *matchServer(t_Request request);
	void buildResponse(Connection *client);
	void readySendResponse(Connection *client, t_epoll &epoll);
	void sendResponse(Connection *client, t_epoll &epoll);

public:
	WebServer();
	~WebServer();

	void setServers(std::vector<Server *> &servers);

	void serve();

	friend std::ostream &operator<<(std::ostream &stream, const WebServer &ws);
};
