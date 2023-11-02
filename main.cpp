#include"include/server.hpp"
#include <sys/socket.h>

int main(int ac, char **av)
{
	Server server;
	int port;

	if(ac != 3)
		return 1;

	port = std::stoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Error port number\n";
		return 1;
	}

	if(!server.initializeServer(port)) {
		std::cout << "Failed to initialize the server.\n";
		return 1;
	}
	return 0;
}
