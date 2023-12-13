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
#include <queue>
#include <sstream>
#include <map>
#include <netdb.h>

#define SERVER_NAME "super_duoIRC"

class Client;
//prototypes for orphan functions
	//error functions
void	fatal_error(std::string err_msg);
void	normal_error(std::string err_msg);
void	client_error(std::string err_msg);
	//parsing functions
void	parse(const std::string& port,const std::string& password);
int		parse_command(std::string command);

//IRC command functions
	//processing input functions
void	process_command(std::string buffer, Client &client);
std::vector<std::string> process_single_command(std::string command);
	//executing commands
int	execute_command(std::string command);
int	pass_command(std::vector<std::string> command);

//util functions
void 	splitString(std::string input, std::string delimiter, std::queue<std::string> &result);
void	clear_buffer(char *buffer, int size);
#endif
