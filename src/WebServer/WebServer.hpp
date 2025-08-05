#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "utils/epoll.hpp"
#include "Connection/Connection.hpp"
#include "Server/Server.hpp"
#include "utils/Result/Result.hpp"

#define READ_BUFFER_SIZE 4096
#define TIMEOUT 5000

class WebServer
{
private:
	std::vector<Server *> servers_;
	std::vector<Connection *> connections_;

	std::vector<fd_t> createServerFds();

	void setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds);
	void setEpollRead(t_epoll &epoll, Connection *connection);
	void setEpollWrite(t_epoll &epoll, Connection *connection);

	fd_t getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd);

	void acceptNewConnection(fd_t &serverFd, t_epoll &epoll);
	std::vector<Connection *>::iterator disconnectConnection(Connection *connection, t_epoll &epoll, const std::string &reason);
	std::vector<Connection *>::iterator removeConnection(Connection *connection);
	void checkConnectionEvent(t_epoll &epoll, const int &eventIndex);
	void handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll);
	void disconnectTimedoutConnections(t_epoll &epoll);

	void receiveRequest(Connection *connection, t_epoll &epoll);
	void processRequest(Connection *connection, Result<t_Request> &result);
	void sendResponse(Connection *connection, t_epoll &epoll);

public:
	WebServer();
	~WebServer();

	const std::vector<Server *> &getServers() const;
	void setServers(std::vector<Server *> &servers);

	void serve();

	static Server *matchServer(const std::vector<Server *> &servers, std::string hostName);
	friend std::ostream &operator<<(std::ostream &stream, const WebServer &ws);
};
