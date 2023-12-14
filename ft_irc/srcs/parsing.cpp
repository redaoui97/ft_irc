#include "../include/irc.hpp"
#include <cctype>
#include <cstdlib>

void	parse_port(std::string port)
{
	int port_n;

	if (port.empty())
		fatal_error("Invalid port!, Empty One");
	for (size_t i = 0;i < port.length(); ++i) {
		if (!isdigit(port[i])) {
			fatal_error("Invalid port!, Digits Only!");
		}
	}
	port_n = std::atoi(port.c_str());
	if (port_n < 1 || port_n > 65535) {
		fatal_error("Invalid port!, Out of range[1-65535]");
	}
}

void	parse_password(std::string password)
{

	if (password.empty()) {
		fatal_error("Invalid password!, Empty One");
	} else if (password.length() < 8 || password.length() > 20) {
		fatal_error("Invalid password!, lenght required[8-20]");
	}

	for (size_t i = 0;i < password.length(); ++i) {
		if (!isalnum(password[i])) {
			fatal_error("Invalid password!, Alphanumeric only!");
		}
	}
}

bool	valid_nickname(std::string nickname)
{
	if (isdigit(nickname[0]) || nickname[0] == ' ' || nickname[0] == '#' || nickname[0] == ':')
        return (false);
	for(int i = 0;i < nickname.size();i++)
	{
		if (!isalnum(nickname[i]) && nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '{' && nickname[i] != '}' && nickname[i] != '\\' && nickname[i] != '|' && nickname[i] != '@')
				return (false);
    }
	return (true);
}

void	parse(const std::string& port,const std::string& password)
{
	parse_port(port);
	parse_password(password);
}
