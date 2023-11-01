#include"include/server.hpp"
#include <sys/socket.h>

int main(int ac, char **av)
{
	struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;
	socklen_t ClientAddrlen;
	char buffer[500];
	int ServerFd;
	int ClientFd;
	int port;

	//if(ac != 3)
	//	return 1;
	ServerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerFd == -1) {
		return 1;
	}

	port = std::stoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Error port number\n";
		return 1;
	}

	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	if(bind(ServerFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		std::cout << "Error Binding";
		return 1;
	}

	if(listen(ServerFd, 10) == -1) {
		std::cout << "Error listening\n";
		return 1;
	}

	while(1){
	ClientAddrlen = sizeof(ClientAddr);
	ClientFd = accept(ServerFd, (sockaddr*)&ClientAddr, &ClientAddrlen);
	bzero(buffer, 500);
	recv(ClientFd, buffer, 500, 0);
	std::cout << "buffer :" << buffer << "\n";
	}

	return 1;
}
