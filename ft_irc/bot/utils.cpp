#include "bot.hpp"

bool    validPort(std::string port) {
    int port_n;

    if (port.empty())
        return false;
    for (size_t i = 0;i < port.length(); ++i) {
        if (!isdigit(port[i])) {
            return false;
        }
    }
    port_n = std::atoi(port.c_str());
    if (port_n <= 1024 || port_n > 65535) {
        return false;
    }
    return true;
}
bool    validNickname(std::string nickname) {
    if (isdigit(nickname[0]) || nickname[0] == ' ' || nickname[0] == '#' || nickname[0] == ':')
        return (false);
    for(int i = 0; i < (int)nickname.size(); i++)
    {
        if (!isalnum(nickname[i]) && nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '{' && nickname[i] != '}' && nickname[i] != '\\' && nickname[i] != '|' && nickname[i] != '@')
                return (false);
    }
    return (true);
}
bool    validPassword(std::string password) {
    if (password.empty()) {
        std::cout << "empty password" << std::endl;
        return false;
    } else if (password.length() < 8 || password.length() > 20) {
        std::cout << "password length" << std::endl;
        return false;
    }

    for (size_t i = 0;i < password.length(); ++i) {
        if (!isalnum(password[i])) {
            std::cout << "password alnum" << std::endl;
            return false;
        }
    }
    return true;
}

bool    parseCommands(char** argv) {

    for (int i = 1; i < 5; i++) {
        if (argv[i][0] == '\0') {
            printHelp("Error: empty argument");
            return false;
        }
    }
    if (!validNickname(argv[1])) {
        printHelp("Error: invalid nickname");
        return false;
    }
    else if (!validPort(argv[4])) {
        printHelp("Error: invalid port");
        return false;
    }
    else if (!validPassword(argv[3])) {
        printHelp("Error: invalid password");
        return false;
    }
    return true;
}

void    printHelp(std::string msg) {
    std::cout << msg << std::endl;
}

void	clearBuffer(char *buffer, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		buffer[i] = '\0';
		i++;
	}
}

bool    parseMessage(char *buffer) {

    std::string msg(buffer);
    std::istringstream iss(msg);

    std::string tmp;
    iss >> tmp;
    if (tmp != "./bot\r\n") {
        return false;
    }
    return true;
}