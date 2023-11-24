#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"

int main(int argc, char **argv)
{
	if(argc == 3)
		parse(argv[1], argv[2]);
	else
		fatal_error("Invlid arguments!");
	
	Server server(100, argv[2]);
	//check if it's a valid port

	if(!server.initializeServer(std::stoi(argv[1])))
		fatal_error("Failed to initialize the server!");
	server.startListening();
	return 0;
}
