#include"../include/client.hpp"

void	Client::clientConnections() {

	char buffer[1024];
	ssize_t	bytes;

	bytes = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes == 0)
			std::cout << "Client closed connection" << std::endl;
		else 
			std::cout << "Error occurred during Client connection" << std::endl;
		close(clientFd);
		return ;
	}
	else {
		// here i will handle the messeges and extraxt commands

	}

}

Client::~Client() {
	std::cout << "Client destructor" << std::endl;
	close(clientFd);
}
