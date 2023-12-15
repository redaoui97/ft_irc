#include "../include/irc.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"

static void ft_irc (std::string port, std::string password)
{	
	Server server(password);
	if(!server.initializeServer(atoi(port.c_str())))
		fatal_error("Failed to initialize the server!");
	server.startListening();
}

int main(int argc, char **argv)
{
	try
	{
		if(argc == 3)
			parse(argv[1], argv[2]);
		else
			fatal_error("Invlid arguments!");
		ft_irc(argv[1], argv[2]);
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	
	return 0;
}
