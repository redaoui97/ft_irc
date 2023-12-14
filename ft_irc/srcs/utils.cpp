#include "../include/server.hpp"
#include "../include/client.hpp"

void splitString(std::string input, std::string delimiter, std::queue<std::string> &result)
{
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::string::npos)
    {
        result.push(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }
    result.push(input.substr(start));
}

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