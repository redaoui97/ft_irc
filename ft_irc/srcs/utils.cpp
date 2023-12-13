#include "../include/server.hpp"

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

    while (i < input.size())
    {
        std::tolower (input.at(i));
        i++;
    }
    return (input);
}