#include"include/server.hpp"
#include <sys/socket.h>

int main(int ac, char **av)
{
	Server server;
	struct sockaddr_in ClientAddr;
	socklen_t ClientAddrlen;
	char buffer[500];
	int ServerFd;
	int ClientFd;
	int port;

	if(ac != 3)
		return 1;

	port = std::stoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Error port number\n";
		return 1;
	}

	if(!server.InitializeServer(port, ServerFd)) {
		std::cout << "Failed to initialize the server.\n";
		return 1;
	}

	ClientAddrlen = sizeof(ClientAddr);
	ClientFd = accept(ServerFd, (sockaddr*)&ClientAddr, &ClientAddrlen);
	bzero(buffer, 500);
	recv(ClientFd, buffer, 500, 0);
	std::cout << "buffer :" << buffer << "\n";

	return 0;
}
