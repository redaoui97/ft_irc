#ifndef IRC_HPP
# define IRC_HPP

//don't add unecessary headers
#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <cstdlib> 
#include <cerrno>
#include <cctype>

//prototypes for orphan functions
void	parse(std::string port,std::string password);
void	fatal_error(std::string err_msg);
void	normal_error(std::string err_msg);
#endif
