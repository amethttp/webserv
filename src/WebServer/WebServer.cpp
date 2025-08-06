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
#include "utils/exceptions/Exceptions.hpp"
#include "Connection/Request/RequestFactory/RequestValidator/HostHeaderValidator/HostHeaderValidator.hpp"

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

static fd_t setServerFd(sockaddr_in &serverAddress, std::set<int> serverPorts)
{
	int enable = 1;

	for (std::set<int>::iterator portsIt = serverPorts.begin(); portsIt != serverPorts.end(); ++portsIt)
	{
		serverAddress.sin_port = htons(*portsIt);
		int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
			throw FatalException("Couldn't set SO_REUSEADDR");
		if (socketFd < 0)
			throw FatalException("Couldn't create socket");
		if (bind(socketFd, (struct sockaddr *)(&serverAddress), sizeof(serverAddress)) == -1)
			throw RecoverableException("Couldn't bind socket");
		if (listen(socketFd, SOMAXCONN) == -1)
			throw RecoverableException("Couldn't listen to port");
		return socketFd;
	}
	return -1;
}

std::vector<fd_t> WebServer::createServerFds()
{
	sockaddr_in serverAddress;
	fd_t serverFd;
	std::vector<fd_t> serversFds;

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_family = AF_INET;
	for (std::vector<Server *>::iterator serversIt = servers_.begin(); serversIt != servers_.end();)
	{
		serverFd = -1;
		try
		{
			std::set<int> serverPorts = (*serversIt)->getPorts();
			serverFd = setServerFd(serverAddress, serverPorts);
			serversFds.push_back(serverFd);
		}
		catch(const RecoverableException& e)
		{
			std::cerr << e.what() << " || Server: " << *((*serversIt)->getNames().begin()) << std::endl;
		}
		serversIt++;
	}
	return serversFds;
}

void WebServer::setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds)
{
	epoll.fd = epoll_create1(0);
	if (epoll.fd == -1)
		throw FatalException("Couldn't create epoll");

	epoll.eventConfig.events = EPOLLIN;
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it != serversFds.end(); )
	{
		try
		{
			epoll.eventConfig.data.fd = *it;
			if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, *it, &epoll.eventConfig) == -1)
				throw RecoverableException("Couldn't add server fd to epoll");
		}
		catch(const RecoverableException& e)
		{
			std::cerr << e.what() << " Server FD: " << *it << std::endl;
			close(*it);
			it = serversFds.erase(it);
			continue;
		}
		it++;
	}
}

void WebServer::setEpollRead(t_epoll &epoll, Connection *connection)
{
	epoll.eventConfig.events = EPOLLIN;
	epoll.eventConfig.data.ptr = static_cast<Connection *>(connection);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, connection->getFd(), &epoll.eventConfig) == -1)
		throw RecoverableException("Couldn't add POLLIN flag to connection fd");
}

void WebServer::setEpollWrite(t_epoll &epoll, Connection *connection)
{
	epoll.eventConfig.events = EPOLLOUT;
	epoll.eventConfig.data.ptr = static_cast<void *>(connection);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, connection->getFd(), &epoll.eventConfig) == -1)
		throw RecoverableException("Couldn't add POLLOUT flag to connection fd");
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
		try
		{
			bzero(&newConnectionAddress, sizeof(newConnectionAddress));
			fd_t newConnectionFd = accept(serverFd, (struct sockaddr *)&newConnectionAddress, &socketSize);
			if (newConnectionFd < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				throw RecoverableException("Couldn't accept new connection");
			}
			if (fcntl(newConnectionFd, F_SETFL, O_NONBLOCK) < 0)
			{
				close(newConnectionFd);
				throw RecoverableException("Couldn't set NONBLOCKING flag to connection fd");
			}
			Connection *newConnection = new Connection();
			newConnection->setFd(newConnectionFd);
			epoll.eventConfig.data.ptr = newConnection;
			epoll.eventConfig.events = EPOLLIN;
			if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, newConnectionFd, &epoll.eventConfig) == -1)
			{
				close(newConnectionFd);
				delete newConnection;
				throw RecoverableException("Couldn't add connection fd to epoll");
			}
			this->connections_.push_back(newConnection);
			std::cout << "New Connection (ID: " << newConnection->getId() << ") connected" << std::endl;
		}
		catch(const RecoverableException& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}

std::vector<Connection *>::iterator WebServer::disconnectConnection(Connection *connection, t_epoll &epoll, const std::string &reason)
{
	if (epoll_ctl(epoll.fd, EPOLL_CTL_DEL, connection->getFd(), NULL) == -1)
		throw FatalException("Couldn't delete connection fd from epoll");
	if (close(connection->getFd()) == -1)
		throw FatalException("Couldn't close connection fd");
	std::cout << "Connection (ID: " << connection->getId() << ") " << reason << std::endl;

	return removeConnection(connection);
}

Server *WebServer::matchServer(const std::vector<Server *> &servers, std::string hostHeader)
{
	std::string hostName = HostHeaderValidator::getHostName(hostHeader);
	for (std::vector<Server *>::const_iterator serverIt = servers.begin(); serverIt != servers.end(); ++serverIt)
	{
		if ((*serverIt)->matchesName(hostName))
			return *serverIt;
	}

	return NULL;
}

void WebServer::processRequest(Connection *connection, Result<t_Request> &result)
{
	try
	{
		Context context;
		t_HandlingResult handlingResult;

		if (result.isSuccess())
		{
			connection->setRequest(result.getValue());
			context.init(this->getServers(), connection->getRequest());
			handlingResult = RequestHandler::handleRequest(context);

			connection->buildResponse(handlingResult);
		}
		else
			connection->buildResponse(result.getError(), C_CLOSE);
	}
	catch(const RecoverableException& e)
	{
		std::cerr << e.what() << std::endl;
		if (e.what() == (std::string)NO_SERVER_MATCH)
			connection->buildResponse(BAD_REQUEST, C_CLOSE);
		else if (e.what() == (std::string)NO_LOCATION_MATCH)
			connection->buildResponse(NOT_FOUND, C_CLOSE);
		else
			connection->buildResponse(INTERNAL_SERVER_ERROR, C_CLOSE);
	}
	connection->clearRequestBuffer();
}

void WebServer::receiveRequest(Connection *connection, t_epoll &epoll)
{
	int bytesReceived;
	char buffer[READ_BUFFER_SIZE + 1];

	bzero(buffer, sizeof(buffer));
	bytesReceived = recv(connection->getFd(), buffer, READ_BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		connection->updateLastReceivedPacket(buffer);
		if (!RequestFactory::canCreateAResponse(connection->getRequestBuffer()))
			return;

		Result<t_Request> result = RequestFactory::create(connection->getRequestBuffer());
		this->processRequest(connection, result);
		this->setEpollWrite(epoll, connection);
	}
	else if (bytesReceived == 0)
		disconnectConnection(connection, epoll, DISCONNECTED);
	else
		throw RecoverableException("Couldn't receive data from connection fd");
}

void WebServer::sendResponse(Connection *connection, t_epoll &epoll)
{
	ssize_t bytesSent = send(connection->getFd(), connection->getResponseBuffer().c_str(), connection->getResponseBuffer().length(), 0);

	if (bytesSent < 0)
		throw RecoverableException("Couldn't send response");

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

	try
	{
		if (epoll.eventBuffer[eventIndex].events & EPOLLIN)
			receiveRequest(connection, epoll);
		else if (epoll.eventBuffer[eventIndex].events & EPOLLOUT)
			sendResponse(connection, epoll);
		else
			disconnectConnection(connection, epoll, DISCONNECTED);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		disconnectConnection(connection, epoll, DISCONNECTED);
	}

}

void WebServer::handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll)
{
	int readyFds;

	std::cout << "Waiting for connections..." << std::endl;
	while (true)
	{
		readyFds = epoll_wait(epoll.fd, epoll.eventBuffer, EVENT_BUFFER_SIZE, TIMEOUT);
		if (readyFds == -1)
			throw FatalException("Couldn't wait for epoll fds");
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
				this->setEpollWrite(epoll, *it);
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

const std::vector<Server *> &WebServer::getServers() const
{
    return this->servers_;
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
