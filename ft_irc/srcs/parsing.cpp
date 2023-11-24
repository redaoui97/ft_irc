#include "irc.hpp"

static int parse_port(std::string port)
{
	(void)port;
	return (EXIT_SUCCESS);
}

static int parse_password(std::string password)
{
	(void)password;
	return(EXIT_SUCCESS);
}

void	parse(std::string port,std::string password)
{
	if (parse_port(port) == EXIT_FAILURE)
		fatal_error("Invalid port number!");
	if (parse_password(password) == EXIT_FAILURE)
		fatal_error("Invalid password!");
}