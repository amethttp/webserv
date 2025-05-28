#include <iostream>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{
	std::cout << "Hello world" << std::endl;

	sockaddr_in	serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	listen(serverSocket, INT_MAX);

	return 1;
}
