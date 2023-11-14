#include"include/server.hpp"
#include <sys/socket.h>

int main(int ac, char **av) {

	if(ac != 3) {

		std::cout << "Error Usage : <" << av[0] << "> <Port> <pasword>" << std::endl;
		return 1;
	}

	Server server(100, av[2]);
	int port;

	port = std::stoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Error port number\n";
		return 1;
	}

	if(server.initializeServer(port)) {
		server.startListening();
	} else {
		std::cout << "Failed to initialize the server.\n";
		return 1;
	}
	return 0;
}
