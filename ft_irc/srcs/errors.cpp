#include "../include/irc.hpp"

void	normal_error(std::string err_msg)
{
	std::cerr << "Error: " << err_msg << std::endl;
}

void	fatal_error(std::string err_msg)
{
	std::cerr << "Fatal error: " << err_msg << std::endl;
	exit(EXIT_FAILURE);
}

void	client_error(std::string err_msg)
{
	//send rpl error message
	(void)err_msg;
}