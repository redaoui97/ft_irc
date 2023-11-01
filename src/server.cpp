#include"../include/server.hpp"
#include <sys/socket.h>

bool	Server::InitializeServer(int port, int &ServerFd)
{
	struct sockaddr_in ServerAddr;

	ServerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerFd == -1) {
		std::cout << "Error at Creation of server socket\n";
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(ServerFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		std::cout << "Error Binding";
		close(ServerFd);
		return false;
	}

	if(listen(ServerFd, 10) == -1) {
		std::cout << "Error listening\n";
		close(ServerFd);
		return 1;
	}
	return true;
}


