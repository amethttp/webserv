#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cerrno>

int main(void)
{
	sockaddr_in	serverAddress, client;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int c = sizeof(struct sockaddr_in);
	int	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Bind failed: " << errno << std::endl;
		return 1;
	}
	listen(serverSocket, SOMAXCONN);
	
	int	dots = 1;
	int bytesRead;
	char buffer[2048];
	epoll_event evSetter;
	epoll_event events[1024];
	int nfds;
	int	epfd = epoll_create1(0);
	
	evSetter.events = EPOLLIN;
	evSetter.data.fd = serverSocket;

	epoll_ctl(epfd, EPOLL_CTL_ADD, serverSocket, &evSetter);
	while (true) {
		std::cout << "Epoll waiting";
		for (int i = 0; i < dots; ++i) {
			std::cout << ".";
		}
		std::cout << std::endl;
		dots = (dots + 1) % 4;

		nfds = epoll_wait(epfd, events, 1024, -1);
		for (int i = 0; i < nfds; ++i) {
			if (events[i].data.fd == serverSocket) {
				memset(&client, 0, sizeof(client));
				evSetter.data.fd = accept(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c);
				if (evSetter.data.fd < 0)
				{
					std::cerr << "Accept failed: " << errno << std::endl;
					return 1;
				}
				fcntl(evSetter.data.fd, F_SETFL, O_NONBLOCK);
				epoll_ctl(epfd, EPOLL_CTL_ADD, evSetter.data.fd, &evSetter);
				std::cout << "Connection accepted" << std::endl;
			} else {
				std::cout << "Existing Client" << std::endl;
				if (events[i].events & EPOLLIN) {
					if ((bytesRead = recv(events[i].data.fd , buffer, 2048 , 0)) > 0) {
						if (strstr(buffer, "exit")) {
							close(serverSocket);
    						close(epfd);
							return 0;
						}
						write(events[i].data.fd , buffer , bytesRead);
						bzero(buffer, sizeof(buffer));
					}
					if (bytesRead == 0) {
						epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
						close(events[i].data.fd);
						std::cout << "Client disconnected" << std::endl;
						continue ;
					} else if (bytesRead == -1) {
						std::cerr << "Recv failed: " << errno << std::endl;
						continue ;
					}
					std::cout << "Response Successfull!" << std::endl;
				}
			}
		}
	}

	return 0;
}
