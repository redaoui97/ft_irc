#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"

int main(int ac, char **av) {

	if(ac != 3)
		return 1;

	//parsing
	Server server(100, av[2]);
	int port;

	port = std::stoi(av[1]);
	if (port < 0 || port > 65535) {
		std::cout << "Error port number\n";
		return 1;
	}

	if(!server.initializeServer(port)) {
		std::cout << "Failed to initialize the server.\n";
		return 1;
	}
	server.startListening();
	return 0;
}
