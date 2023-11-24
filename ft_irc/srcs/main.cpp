#include "irc.hpp"
#include "server.hpp"
#include "client.hpp"

static void ft_irc (std::string port, std::string password)
{	
	Server server(password);
	if(!server.initializeServer(std::stoi(port)))
		fatal_error("Failed to initialize the server!");
	server.startListening();
}

int main(int argc, char **argv)
{
	if(argc == 3)
		parse(argv[1], argv[2]);
	else
		fatal_error("Invlid arguments!");
	ft_irc(argv[1], argv[2]);
	return 0;
}
