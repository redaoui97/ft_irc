#include "../include/server.hpp"

//preparing the input 
void	process_command(std::string buffer, Client &client)
{
	std::queue<std::string>  commands_queue;
	std::vector<std::string> prepared_command;
	std::string				 command;

	splitString(buffer, "\r\n", commands_queue);
	while (!commands_queue.empty())
	{
		command = commands_queue.front();
		commands_queue.pop();
		std::cout << "command: " << command << std::endl;
		prepared_command = process_single_command(command);
	}
}

std::vector<std::string> process_single_command(std::string command)
{
	std::vector<std::string> command_vector;
    std::istringstream iss(command);
    std::string token;
	std::string iteration_token;
	bool		command_found = false;

	//need to fix parser -> many spaces break the trailing and takes every space as arguemnt -> ':' shouldn't be part of the param

    while (std::getline(iss, token, ' '))
	{
		if (!command_found)
		{
			if (!look_up_command(token))
				client_error("error(421): " + token + "Unknown command");
		}
        if (!command_vector.empty() && command_vector.back().front() == ':' && !token.empty())
		{
            command_vector.back() += ' ' + token;
        }
		else
		{
            command_vector.push_back(token);
        }
    }
	int i = 0;
	while (i < (int)command_vector.size())
	{
		std::cout << "arg" << i << ": " << command_vector.at(i) << std::endl;
		i++;
	}
    return (command_vector);
}

//attributing the appropriate function
bool	look_up_command(std::string command_name)
{
	std::string commands[] = {"command1", "command2"};
	int			i = 0;

	while (i < (int)(sizeof(commands)/sizeof(std::string)))
	{
		if ()
		i++;
	}
	return (false);
}
// int	execute_command(std::string command)
// {
// 	int	return_value = EXIT_SUCCESS;


// }

//execute the function
//authentication commands
// int	pass_command(std::vector<std::string> command)
// {
// 	return (EXIT_SUCCESS);
// }