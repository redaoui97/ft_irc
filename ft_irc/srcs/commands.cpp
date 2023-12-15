#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/channel.hpp"

//preparing the input 
void    process_command(std::string buffer, Client *client, std::string password)
{
    std::queue<std::string>  commands_queue;
    std::vector<std::string> prepared_command;
    std::string              command;

    splitString(buffer, "\r\n", commands_queue);
    while (!commands_queue.empty())
    {
        command = commands_queue.front();
        commands_queue.pop();
        std::cout << "command: " << command << std::endl;
        if (command.size())
        {
            prepared_command = process_single_command(command);
            execute_commands(prepared_command, client, password);
        }
    }
}

std::vector<std::string> process_single_command(std::string command)
{
    std::vector<std::string> command_vector;
    std::istringstream iss(command);
    std::string token;
    std::string iteration_token;

    //need to fix parser -> many spaces break the trailing and takes every space as arguemnt -> ':' shouldn't be part of the param
    //this function will be replaced by the trailing handler function by athmane 
    while (std::getline(iss, token, ' '))
    {
        if (!command_vector.empty() && command_vector.back().front() == ':' && !token.empty())
        {
            command_vector.back() += ' ' + token;
        }
        else
        {
            command_vector.push_back(token);
        }
    }
    return (command_vector);
}


//execute the function
void execute_commands(std::vector<std::string>args, Client* client, std::string password)
{
    // if ((args.front()).compare("QUIT") == 0)
        //quit_cmd(client, args);
    if (!client->IsAuthenticated())
    {
        if (!(args.front()).compare("PASS") || !(args.front()).compare("NICK") || !(args.front()).compare("USER"))
            authentication(args, client, password);
        else
            send_err(client, ERR_NOTREGISTERED, ":You have not registered");
    }
    else
    {
        std::cout << "executing command while authenticated" << std::endl;
        if (!(args.front()).compare("PRIVMSG"))
        {
            std::cout << "supposed to execute privmsg" << std::endl;
            privmsg_cmd(client, args);
        }
        else
        {
            std::cout << "unknown command" << std::endl;
            send_message((":" + host_name() + " 421 " + client->getNickname() + " " + args.at(0) + " :Unkown command"), client);
            return ;
        }
    }
}

//authentication commands
void authentication(std::vector<std::string>args, Client* client, std::string password)
{
    if ((args.front()).compare("PASS") == 0)
        pass_cmd(client, args, password);
    else if (!client->HasRightPassword())
    {
        send_err(client, ERR_PASSWDMISMATCH, ":Password incorrect, please authenticate first");
    }
    else
    {
        if ((args.front()).compare("NICK") == 0)
        {
            nick_cmd(client, args);
        }
        else if ((args.front()).compare("USER") == 0)
        {
            user_cmd(client, args);
        }
        else
        {
            send_err(client, ERR_NOTREGISTERED, ":You have not registered");
        }
    }
}

void nick_cmd(Client *client, std::vector<std::string> args)
{
    if(args.size() < 2)
    {
        send_err(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
        return ;
    }
    if (!valid_nickname(args.at(1)))
    {
        send_err(client, ERR_ERRONEUSNICKNAME, ":Erroneus nickname");
        return ;
    }
    if (client->GetServer()->client_exists(args.at(1)))
    {
        send_message(":" + host_name() + " " + args.at(1) + " 433 * " + ":Nickname is already in use" + "\r\n", client);
        return ;
    }
    //I haven't tested this yet. Test it after authentication
    if (client->IsAuthenticated())
    {
        send_message(":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + ".ip NICK :" +args.at(1), client);
    }
    client->setNickname(args.at(1));
}

void user_cmd(Client *client, std::vector<std::string> args)
{
    if ((client->getNickname()).empty())
        return ;
    if (args.size() < 5)
    {
        send_message(":" + host_name() + " " + args.at(0) + " 461 " + ":Not enough parameters" + "\r\n", client);
        return ;
    }
    if (client->IsAuthenticated())
    {
        send_err(client, ERR_ALREADYREGISTERED, ":You may not reregister");
        return ;
    }
    client->setRealname(args.at(4));
    client->setUsername(args.at(1));
    client->AuthenticationStatus(true);
    trigger_welcome(client);
}

void trigger_welcome(Client *client)
{
    send_message((":" + host_name() + " 001 " + client->getNickname() + " :Welcome to the " + SERVER_NAME + " " + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + "\r\n"), client);
    send_message((":" + host_name() + " 002 " + client->getNickname() + " :Your host is " + host_name() +", running version " + (client->GetServer())->get_version() + "\r\n"), client);
    send_message((":" + host_name() + " 003 " + client->getNickname() + " This server was created " + (client->GetServer())->get_time() + " UTC" + "\r\n"), client);
}

void pass_cmd(Client *client, std::vector<std::string> args, std::string password)
{
    if (args.size() != 2)
    {
        send_err(client, ERR_NEEDMOREPARAMS, ":Not enough parameters");
        return ;
    }
    if (client->IsAuthenticated())
    {
        send_err(client, ERR_ALREADYREGISTERED, ":You may not reregister");
        return ;
    }
    else
    {
        if (args.at(1).compare(password))
            send_err(client, ERR_PASSWDMISMATCH, ":Password incorrect");
        else
        {
            client->SetRightPassword(true);
        }
    }
}


//channel commands
void    join_cmd(Client *client, std::string channel_name)
{
    (void)client;
    (void)channel_name;
    //(client->GetServer())
}

//other commands
void privmsg_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() < 2)
    {
        send_message((":" + host_name() + " 412 " + client->getNickname() + " :No text to send"), client);
        return ;
    }
    //check channels before checking users
    if (!(client->GetServer())->client_exists(args.at(1)))
    {
        send_message((":" + host_name() + " 401 " + client->getNickname() + " " + args.at(1) + " :No such nick/channel"), client);
        return ;
    }
}
// void quit_cmd(Client *client, std::vector<std::string> args)
// {

// }
