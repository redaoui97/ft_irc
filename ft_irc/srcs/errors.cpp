#include "../include/irc.hpp"

void	normal_error(std::string err_msg)
{
	std::cout << "Error: " << err_msg << std::endl;
}

void	fatal_error(std::string err_msg)
{
	std::cout << "Fatal error: " << err_msg << std::endl;
	exit(EXIT_FAILURE);
}
