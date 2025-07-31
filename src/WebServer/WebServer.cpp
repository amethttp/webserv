#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "WebServer.hpp"
#include "Connection/Connection.hpp"
#include "Connection/Request/RequestFactory/RequestFactory.hpp"

WebServer::WebServer()
{
}

WebServer::~WebServer()
{
	for (std::vector<Connection *>::iterator it = connections_.begin(); it != connections_.end(); ++it)
		delete *it;
	this->connections_.clear();

	for (std::vector<Server *>::iterator it = servers_.begin(); it != servers_.end(); ++it)
		delete *it;
	this->servers_.clear();
}

std::vector<fd_t> WebServer::createServerFds()
{
	sockaddr_in serverAddress;
	std::vector<fd_t> serversFds;
	// TODO: ERASE THIS VAR
	int enable = 1;

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_family = AF_INET;
	for (std::vector<Server *>::iterator serversIt = servers_.begin(); serversIt != servers_.end(); ++serversIt)
	{
		std::set<int> serverPorts = (*serversIt)->getPorts();
		for (std::set<int>::iterator portsIt = serverPorts.begin(); portsIt != serverPorts.end(); ++portsIt)
		{
			serverAddress.sin_port = htons(*portsIt);
			int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
			// TODO: ERASE SETSOCKOPT
			if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
				throw std::runtime_error("Couldn't set SO_REUSEADDR");
			if (socketFd < 0)
				throw std::runtime_error("Couldn't create socket");
			if (bind(socketFd, (struct sockaddr *)(&serverAddress), sizeof(serverAddress)) == -1)
				throw std::runtime_error("Couldn't bind socket");
			if (listen(socketFd, SOMAXCONN) == -1)
				throw std::runtime_error("Couldn't listen to port");
			serversFds.push_back(socketFd);
		}
	}
	return serversFds;
}

void WebServer::setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds)
{
	epoll.fd = epoll_create1(0);
	if (epoll.fd == -1)
		throw std::runtime_error("Couldn't create epoll");

	epoll.eventConfig.events = EPOLLIN;
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it != serversFds.end(); ++it)
	{
		epoll.eventConfig.data.fd = *it;
		if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, *it, &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add server fd to epoll");
	}
}

void WebServer::setEpollRead(t_epoll &epoll, Connection *connection)
{
	epoll.eventConfig.events = EPOLLIN;
	epoll.eventConfig.data.ptr = static_cast<Connection *>(connection);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, connection->getFd(), &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add POLLIN flag to connection fd");
}

void WebServer::setEpollWrite(t_epoll &epoll, Connection *connection)
{
	epoll.eventConfig.events = EPOLLOUT;
	epoll.eventConfig.data.ptr = static_cast<void *>(connection);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, connection->getFd(), &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add POLLOUT flag to connection fd");
}

fd_t WebServer::getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd)
{
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it != serversFds.end(); ++it)
	{
		if (eventFd == *it)
			return *it;
	}
	return -1;
}

void WebServer::acceptNewConnection(fd_t &serverFd, t_epoll &epoll)
{
	sockaddr_in newConnectionAddress;
	socklen_t socketSize = sizeof(newConnectionAddress);

	while (true)
	{
		bzero(&newConnectionAddress, sizeof(newConnectionAddress));
		fd_t newConnectionFd = accept(serverFd, (struct sockaddr *)&newConnectionAddress, &socketSize);
		if (newConnectionFd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			throw std::runtime_error("Couldn't accept new connection");
		}
		if (fcntl(newConnectionFd, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Couldn't set NONBLOCKING flag to connection fd");
		Connection *newConnection = new Connection();
		this->connections_.push_back(newConnection);
		newConnection->setFd(newConnectionFd);
		epoll.eventConfig.data.ptr = newConnection;
		epoll.eventConfig.events = EPOLLIN;
		if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, newConnectionFd, &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add connection fd to epoll");
		std::cout << "New Connection (ID: " << newConnection->getId() << ") connected" << std::endl;
	}
}

std::vector<Connection *>::iterator WebServer::disconnectConnection(Connection *connection, t_epoll &epoll, const std::string &reason)
{
	if (epoll_ctl(epoll.fd, EPOLL_CTL_DEL, connection->getFd(), NULL) == -1)
		throw std::runtime_error("Couldn't delete connection fd from epoll");
	if (close(connection->getFd()) == -1)
		throw std::runtime_error("Couldn't close connection fd");
	std::cout << "Connection (ID: " << connection->getId() << ") " << reason << std::endl;

	return removeConnection(connection);
}

bool WebServer::tryBuildRequest(Connection *connection, const char *buffer)
{
	connection->appendToRequestBuffer(buffer);
	if (RequestFactory::canCreateAResponse(connection->getRequest().buffer) == false)
		return false;

	connection->buildRequest(connection->getRequest().buffer.c_str());
	return true;
}

Server *WebServer::matchServer(t_Request request)
{
	std::string hostName = request.headers.getHeaderValue(HOST);

	for (std::vector<Server *>::iterator serverIt = servers_.begin(); serverIt != servers_.end(); ++serverIt)
	{
		if ((*serverIt)->matchesName(hostName))
			return *serverIt;
	}

	return *servers_.begin();
}

void WebServer::buildResponse(Connection *connection)
{
	Server *server;
	Location *location;

	connection->getRequest().buffer.clear();
	server = this->matchServer(connection->getRequest());
	location = server->matchLocation(connection->getRequest());

	connection->buildResponse(server, location);
}

void WebServer::readySendResponse(Connection *connection, t_epoll &epoll)
{
	// TO DO: Check if clear request needed
	
	setEpollWrite(epoll, connection);
}

void WebServer::receiveRequest(Connection *connection, t_epoll &epoll)
{
	int bytesReceived;
	char buffer[READ_BUFFER_SIZE + 1];

	bzero(buffer, sizeof(buffer));
	bytesReceived = recv(connection->getFd(), buffer, READ_BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		connection->updateLastReceivedPacket();
		if (!this->tryBuildRequest(connection, buffer))
			return;
		this->buildResponse(connection);
		this->readySendResponse(connection, epoll);
	}
	else if (bytesReceived == 0)
		disconnectConnection(connection, epoll, DISCONNECTED);
	else
		throw std::runtime_error("Couldn't receive data from connection fd");
}

void WebServer::sendResponse(Connection *connection, t_epoll &epoll)
{
	ssize_t bytesSent = send(connection->getFd(), connection->getResponseBuffer().c_str(), connection->getResponseBuffer().length(), 0);

	if (bytesSent < 0)
		throw std::runtime_error("Couldn't send response");

	connection->eraseResponse(bytesSent);
	if ((ssize_t)connection->getResponseBuffer().length())
		return;

	if (connection->shouldClose())
	{
		std::string reason = DISCONNECTED;
		if (connection->getResponseStatus() == REQUEST_TIME_OUT)
			reason = TIMED_OUT;
		disconnectConnection(connection, epoll, reason);
	}
	else
		setEpollRead(epoll, connection);
}

void WebServer::checkConnectionEvent(t_epoll &epoll, const int &eventIndex)
{
	Connection *connection = static_cast<Connection *>(epoll.eventBuffer[eventIndex].data.ptr);

	// struct sockaddr_in addr;
	// socklen_t len = sizeof(addr);
	// getsockname(connection->getFd(), (struct sockaddr*)&addr, &len);
	// int local_port = ntohs(addr.sin_port);

	if (epoll.eventBuffer[eventIndex].events & EPOLLIN)
		receiveRequest(connection, epoll);
	else if (epoll.eventBuffer[eventIndex].events & EPOLLOUT)
		sendResponse(connection, epoll);
}

void WebServer::handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll)
{
	int readyFds;

	std::cout << "Waiting for connections..." << std::endl;
	while (true)
	{
		readyFds = epoll_wait(epoll.fd, epoll.eventBuffer, EVENT_BUFFER_SIZE, TIMEOUT);
		if (readyFds == -1)
			throw std::runtime_error("Couldn't wait for epoll fds");
		for (int i = 0; i < readyFds; i++)
		{
			fd_t serverFd = getServerFd(serversFds, epoll.eventBuffer[i].data.fd);
			if (serverFd != -1)
				acceptNewConnection(serverFd, epoll);
			else
				checkConnectionEvent(epoll, i);
		}
		disconnectTimedoutConnections(epoll);
	}
}

std::vector<Connection *>::iterator WebServer::removeConnection(Connection *connection)
{
	int connectionId = connection->getId();

	for (std::vector<Connection *>::iterator it = this->connections_.begin(); it != this->connections_.end(); ++it)
	{
		if ((*it)->getId() == connectionId)
		{
			delete *it;
			return this->connections_.erase(it);
		}
	}

	return this->connections_.end();
}

void WebServer::disconnectTimedoutConnections(t_epoll &epoll)
{
	time_t now = std::time(NULL);

	for (std::vector<Connection *>::iterator it = connections_.begin(); it != connections_.end();)
	{
		if ((now - (*it)->getLastReceivedPacket()) * 1000 > TIMEOUT)
		{
			if ((*it)->hasPendingRequest())
			{
				(*it)->buildResponse(REQUEST_TIME_OUT, C_CLOSE);
				this->readySendResponse(*it, epoll);
			}
			else
			{
				it = disconnectConnection(*it, epoll, TIMED_OUT);
				continue;
			}
		}
		++it;
	}
}

void WebServer::setServers(std::vector<Server *> &servers)
{
	this->servers_ = servers;
}

void WebServer::serve()
{
	if (this->servers_.empty())
		return;

	t_epoll epoll;
	std::vector<fd_t> serversFds;

	serversFds = createServerFds();
	setEpollInstance(epoll, serversFds);
	handleConnectionEvents(serversFds, epoll);
}

std::ostream &operator<<(std::ostream &stream, const WebServer &ws)
{
	stream << "--- WEBSERVER ---" << std::endl;
	for (std::vector<Server *>::const_iterator it = ws.servers_.begin(); it != ws.servers_.end(); ++it)
		stream << **it << "\r\n";
	stream << "\r\n";
	return stream;
}
