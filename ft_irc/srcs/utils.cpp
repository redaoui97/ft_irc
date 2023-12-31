#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/irc.hpp"

void	clear_buffer(char *buffer, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		buffer[i] = '\0';
		i++;
	}
}

std::string get_date()
{
    char buffer[80];
    time_t rawTime;
    struct tm* timeInfo;

    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, sizeof(buffer), "%c %Z", timeInfo);
    return (buffer);
}

std::string	to_String(int n)
{
	std::stringstream	ss;

	ss << n;
	return (ss.str());
}

std::string tolower(std::string input)
{
    int i = 0;

    while (i < (int)input.size())
    {
        std::tolower (input.at(i));
        i++;
    }
    return (input);
}

void send_message(std::string msg, Client *client)
{
    size_t sent = 0;
	while (sent != msg.length())
	{
		ssize_t send_status = send(client->getClientFd() ,msg.c_str() + sent ,msg.length() - sent, 0);
		if (send_status == -1)
			std::cout << "sending error" << std::endl; // to check later 
		sent += send_status;
	}
}

void broadcast_message(std::string msg, std::map<std::string, Client *> clients)
{

    for (std::map<std::string, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        Client* currentClient = it->second;
        if (currentClient)
            send_message(msg, currentClient);
    }
}

std::string host_name()
{
    char host[253];
    if (gethostname(host, sizeof(host)) == -1)
        return "";
    return host;
}

void    send_err(Client *client,  err_replies rep, std::string additional_message)
{
    std::string message;

    switch (rep)
    {
        case ERR_PASSWDMISMATCH:
            message = (":" + host_name() + " 464 * " + additional_message + "\r\n");
            break;
        case ERR_ALREADYREGISTERED:
            message = (":" + host_name() + " 462 * " + additional_message + "\r\n");
            break;
        case ERR_NEEDMOREPARAMS:
            message = (":" + host_name() + " 461 * " + additional_message + "\r\n");
            break;
        case ERR_NONICKNAMEGIVEN:
            message = (":" + host_name() + " 431 * " + additional_message + "\r\n");
            break;
        case ERR_ERRONEUSNICKNAME:
            message = (":" + host_name() + " 432 * " + additional_message + "\r\n");
            break;
        case ERR_NOTREGISTERED:
            message = (":" + host_name() + " 451 * " + additional_message + "\r\n");
            break;
        default :
            message = "hh";
            break;
    } 
    send_message(message, client);
}

static bool isAlphabetic(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string trimSpaces(const std::string& str)
{
    // Find the first non-alphabetic character from the beginning
    std::string::size_type start = 0;
    while (start < str.length() && !isAlphabetic(str[start])) {
        ++start;
    }

    // Check for an empty string or all non-alphabetic characters
    if (start == str.length())
        return "";

    // Find the last non-alphabetic character from the end
    std::string::size_type end = str.length() - 1;
    while (end > 0 && !isAlphabetic(str[end])) {
        --end;
    }

    // Extract the substring containing alphabetic characters
    return str.substr(start, end - start + 1);
}

std::string trimPoints(const std::string& str)
{
    std::string::size_type start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    std::string::size_type end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}