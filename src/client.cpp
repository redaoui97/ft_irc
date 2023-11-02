#include"../include/client.hpp"

void	Client::ClientConnections() {

	char buffer[1024];
	ssize_t	bytes;

	bytes = recv(ClientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes == 0)
			std::cout << "Client closed connection" << std::endl;
		else 
			std::cout << "Error occurred during Client connection" << std::endl;
		close(ClientFd);
		return ;
	}
	else {
		// here i will handle the messeges and extraxt commands

	}

}
