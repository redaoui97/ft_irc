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
        if (!(args.front()).compare("PRIVMSG"))
        {
            privmsg_cmd(client, args);
        }
        if (!(args.front()).compare("JOIN"))
        {
            join_cmd(client, args);
        }
        if (!(args.front()).compare("KICK") || !(args.front()).compare("INVITE") || !(args.front()).compare("TOPIC") || !(args.front()).compare("MODE"))
        {
            mod_commands(args, client);
        }
        else
        {
            std::cout << "unknown command" << std::endl;
            send_message((":" + host_name() + " 421 " + client->getNickname() + " " + args.at(0) + " :Unkown command", + "\r\n"), client);
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
        send_message(":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + ".ip NICK :" +args.at(1) + "\r\n", client);
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
void    join_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() < 2 || ((args.at(1)).compare("#") == 0))
    {
        send_message((":" + host_name() + " 461 " + client->getNickname() + " JOIN :Not enough parameters" + "\r\n"), client);
        return ;
    }
    if (!(client->GetServer()->channel_exists(args.at(1))))
    {
        if (args.size() > 2)
        {
            (client->GetServer())->new_channel(args.at(1), client, args.at(2));
        }
        else if (args.size() == 2)
        {
            (client->GetServer())->new_channel(args.at(1), client, "");
        }
    }
    else
    {
        channel *chann = client->GetServer()->find_channel(args.at(1));
        if (chann->is_member(client->getNickname()))
        {
            send_message((":" + host_name() + " 443 " + client->getNickname() + " " + args.at(1) + " :is already on channel" + "\r\n"), client);
            return ;
        }
        if (chann->require_invite())
        {
            if (!(chann->is_invited(client->getNickname())))
            {
                send_message((":" + host_name() + " 473 " + client->getNickname() + " " + args.at(1) + " :Cannot join channel" + "\r\n"),client);
                return ;
            }
        }
        if (chann->require_pw())
        {
            if (args.size() > 2 && chann->is_right_pw(args.at(2)))
            {
                chann->add_client(client);
            }
            else
            {
                send_message((":" + host_name() + " 475 " + client->getNickname() + " " + args.at(1) + " :Cannot join channel" + "\r\n"),client);
                return ;
            }
        }
        else
        {
            chann->add_client(client);
        }
        send_message((":" + host_name() + " 332 ", client->getNickname() + " " + args.at(1) + " :Topic: " + chann->get_topic() + "\r\n"), client);


    }
    send_message((":" + host_name() + " 353 " + client->getNickname() + " = " + args.at(1) + " :@" + client->getNickname() + "\r\n"), client);
    send_message((":" + host_name() + " 366 " + client->getNickname() + " " + args.at(1) + " :End of /NAMES list" + "\r\n"), client);
}

void    mod_commands(std::vector<std::string> args, Client *client)
{
    if (!(args.front()).compare("INVITE"))
    {
        invite_commands(args, client);
    }
    if (!(args.front()).compare("KICK"))
    {
        invite_commands(args, client);
    }
    if (!(args.front()).compare("TOPIC"))
    {
        invite_commands(args, client);
    }
    if (!(args.front()).compare("MODE"))
    {
        invite_commands(args, client);
    }
}

void    kick_commands(std::vector<std::string> args, Client *client)
{
    (void)client;
    (void)args;
}

void    invite_commands(std::vector<std::string> args, Client *client)
{

    if (args.size() < 3)
    {
        send_message(":" + host_name() + " " + args.at(0) + " 461 " + ":Not enough parameters" + "\r\n", client);
        return ;
    }
    if (!(client->GetServer())->channel_exists(args.at(2)))
    {
        send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(2) + " :No such channel" + "\r\n"), client);
        return ;
    }

    channel *chann = client->GetServer()->find_channel(args.at(2));
    if (!(chann->is_mod(client->getNickname())))
    {
        send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(2) + " :You're not channel operator" + "\r\n"), client);
        return ;
    }
    if (!(chann->is_member(client->getNickname())))
    {
        send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(2) + " :You're not on that channel" + "\r\n"), client);
        return ;
    }
    if (chann->is_member(client->getNickname()))
    {
        send_message((":" + host_name() + " 443 " + client->getNickname() + " " + args.at(2) + " :is already on channel" + "\r\n"), client);
        return ;
    }
    send_message(":" + client->getNickname() + "!" + client->getUsername() + client->getHostname() + " INVITE " + args.at(1) + ":" + args.at(2) + "\r\n", client);
    Client *client2 = (client->GetServer())->find_user_bynick(args.at(1));
    if (client2)
    {
        send_message((":" + host_name() + " 341 " + args.at(1) + " " + args.at(2) + " :Invite to join channel" + "\r\n"), client);
        chann->add_toinvite(client2);
    }
}

void    topic_commands(std::vector<std::string> args, Client *client)
{
    (void)client;
    (void)args;
}

void    mode_commands(std::vector<std::string> args, Client *client)
{
    char    i = ' ';
    char    t = ' ';
    char    k = ' ';
    char    o = ' ';
    std::string flags;
    std::vector<std::string>::iterator it;
    
    channel *chann = client->GetServer()->find_channel(args.at(1));
    if (!chann->is_mod(client->getNickname()))
    {
        send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(2) + " :You're not channel operator" + "\r\n"), client);
        return ;
    }
    for (it = args.begin(); it != args.end(); ++it)
    {
        if ((*it)[0] != '+' && (*it)[0] != '-')
            continue ;
        for (size_t i = 0; i < (*it).length(); ++i)
        {
            char currentChar = (*it)[i];
            if (currentChar == 'i')
                i = (*it)[0];
            if (currentChar == 't')
                t = (*it)[0];
            if (currentChar == 'k')
                k = (*it)[0];
            if (currentChar == 'o')
                o = (*it)[0];
        }
    }
    if (i != ' ')
    {
        if (i == '+')
        {
            chann->set_inv_status(true);
        }
        else if (i == '-')
        {
            chann->set_inv_status(false);
        }
    }
    if (t != ' ')
    {
        (void)t;
    }
    if (k != ' ')
    {
        (void)k;
    }
    if (o != ' ')
    {
        (void)o;
    }
}

//other commands
void privmsg_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() < 2)
    {
        send_message((":" + host_name() + " 412 " + client->getNickname() + " :No text to send" + "\r\n"), client);
        return ;
    }
    if (args.at(1).at(0) == '#')
    {
        if (!(client->GetServer())->channel_exists(args.at(1)))
        {
            send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(1) + " :No such channel" + "\r\n"), client);
            return ;
        }
    }
    if (!(client->GetServer())->client_exists(args.at(1)))
    {
        send_message((":" + host_name() + " 401 " + client->getNickname() + " " + args.at(1) + " :No such nick/channel"), client);
        return ;
    }
}
// void quit_cmd(Client *client, std::vector<std::string> args)
// {

// }
//void notice_cmd();
