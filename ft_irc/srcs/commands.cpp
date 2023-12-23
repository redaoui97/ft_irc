#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/channel.hpp"

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
        send_message(":" + host_name() + " " + args.at(0) + " 461 " + " USER :Not enough parameters" + "\r\n", client);
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
    else if (args.at(1).compare(password))
    {
        send_err(client, ERR_PASSWDMISMATCH, ":Password incorrect");
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
            send_message("correct password\r\n", client);
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
    std::vector<std::string> tokens = splitString(args.at(1), ',');

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        args.at(1) = tokens[i];
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
            channel *chann = (client->GetServer())->find_channel(args.at(1));
            if (!chann)
                return ;
            if (chann->is_member(client->getNickname()))
            {
                send_message((":" + host_name() + " 443 " + client->getNickname() + " " + args.at(1) + " :is already on channel" + "\r\n"), client);
                continue ;
            }
            if (chann->require_invite())
            {
                if (!(chann->is_invited(client->getNickname())))
                {
                    send_message((":" + host_name() + " 473 " + client->getNickname() + " " + args.at(1) + " :Cannot join channel" + "\r\n"),client);
                    continue ;
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
                    continue ;
                }
            }
            if (chann->get_size() == chann->get_max_size())
            {
                send_message((":" + host_name() + " 471 " + client->getNickname() + " " + args.at(1) + " :Cannot join channel (+l)" + "\r\n"),client);
                return ;
            }
            else
            {
                chann->add_client(client);
            }
        }
        channel *chann = (client->GetServer())->find_channel(args.at(1));
        std::string statuss = " = ";
        std::string clients_connected = client->getNickname();
        if (chann)
        {
            broadcast_message((":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp()+ ".ip JOIN :" + args.at(1) + "\r\n"), chann->all_clients(), NULL);
            statuss = (chann->require_pw() ? " @" : " = ");
            clients_connected = chann->print_all_client();
        }
        send_message((":" + host_name() + " 353 " + client->getNickname() + statuss + args.at(1) + " :@" + clients_connected + "\r\n"), client);
        send_message((":" + host_name() + " 366 " + client->getNickname() + " " + args.at(1) + " :End of /NAMES list" + "\r\n"), client);
        send_message((":" + host_name() + " 332 " + client->getNickname() + " " + args.at(1) + " :" + chann->get_topic() + "\r\n"), client);
    }
}

void    mod_commands(std::vector<std::string> args, Client *client)
{
    if (!(args.front()).compare("INVITE"))
    {
        invite_commands(args, client);
    }
    if (!(args.front()).compare("KICK"))
    {
        kick_commands(args, client);
    }
    if (!(args.front()).compare("TOPIC"))
    {
        topic_commands(args, client);
    }
    if (!(args.front()).compare("MODE"))
    {
        mode_commands(args, client);
    }
}

void    invite_commands(std::vector<std::string> args, Client *client)
{

    if (args.size() < 3)
    {
        send_message(":" + host_name() +  " 461 " + args.at(0) + " :Not enough parameters" + "\r\n", client);
        return ;
    }
    if (!(client->GetServer())->channel_exists(args.at(2)))
    {
        send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(2) + " :No such channel" + "\r\n"), client);
        return ;
    }
    channel *chann = (client->GetServer())->find_channel(args.at(2));
    if (!chann)
        return ;
    Client *client2 = (client->GetServer())->find_user_bynick(args.at(1));
    if (!client2)
    {
        send_message((":" + host_name() + " 401 " + args.at(1) + " :No such nick/channel" + "\r\n"), client);
        return ;
    }
    if (!(chann->is_member(client->getNickname())))
    {
        send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(2) + " :You're not on that channel" + "\r\n"), client);
        return ;
    }
    if (!(chann->is_mod(client->getNickname())))
    {
        send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(2) + " :You're not channel operator" + "\r\n"), client);
        return ;
    }
    if (chann->is_member(client2->getNickname()))
    {
        send_message((":" + host_name() + " 443 " + client->getNickname() + " " + args.at(2) + " :is already on channel" + "\r\n"), client);
        return ;
    }
    send_message(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + ".ip INVITE " + args.at(1) + " :" + args.at(2) + "\r\n", client2);
    send_message(":" + host_name() + " 345 " + client->getNickname() + " " + args.at(2) + " :" + args.at(1) + "\r\n", client2);
    chann->add_toinvite(client2);
}

void    kick_commands(std::vector<std::string> args, Client *client)
{
    if (args.size() < 3 || (args.at(1)).empty() || (args.at(2)).empty())
    {
        send_message(":" + host_name() +  " 461 " + client->getNickname() + " KICK :Not enough parameters" + "\r\n", client);
        return ;
    }
    Client *clt = (client->GetServer())->find_user_bynick(args.at(2));
    if (!clt)
    {
        //split and loop here
        send_message((":" + host_name() + " 401 " + client->getNickname() + " " + args.at(2) + " :No such nick/channel" + "\r\n"), client);
        return ;
    }
    channel *chann = (client->GetServer())->find_channel(args.at(1));
    if (!chann)
    {
        send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(1) + " :No such channel" + "\r\n"), client);
        return ;
    }
    if (!chann->is_member(client->getNickname()))
    {
        send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(1) + " :You're not on that channel" + "\r\n"), client);
        return ;
    }
    if (!chann->is_member(clt->getNickname()))
    {
        send_message((":" + host_name() + " 441 " + clt->getNickname() + " " + args.at(1) + " :They aren't on that channel" + "\r\n"), client);
        return ;
    }
    if (!chann->is_mod(client->getNickname()))
    {
        send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(1) + " :You're not channel operator" + "\r\n"), client);
        return ;
    }

    chann->delete_client(clt);
    if (chann->is_mod(clt->getNickname()))
        chann->remove_mod(clt);
    if (args.size() < 4)
        broadcast_message((":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " KICK " + chann->get_name() + " " + args.at(2) + " :" + client->getNickname() + "\r\n"), chann->all_clients(), NULL);
    else
        broadcast_message((":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " KICK " + chann->get_name() + " " + args.at(2) + " :" + client->getNickname() + "\r\n"), chann->all_clients(), NULL);
        // broadcast_message((":" +  host_name() + " " + client->getNickname() + " " + args.at(1) + " " + args.at(2) + " Kicked by operator " + args.at(3) + "\r\n"), chann->all_clients(),client);
    send_message((":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " KICK " + chann->get_name() + " " + args.at(2) + " :" + client->getNickname() + "\r\n"), clt);
    // send_message((":" + host_name() + " 312 " + client->getNickname() + " " + args.at(1) + " " + clt->getNickname() + " :You were kicked by operator" + "\r\n"), clt);
    if (((chann->all_clients())).size() == 0)
        ((client->GetServer())->delete_channel(chann->get_name()));
    
}

//handling trailing here
//needs testing
void    topic_commands(std::vector<std::string> args, Client *client)
{
    if (args.size() < 2 || (args.size() == 2 && (args.at(1)) == ":"))
    {
        send_message(":" + host_name() +  " 461 " + client->getNickname() + " TOPIC :Not enough parameters" + "\r\n", client);
        return ;
    }
    if (!(client->GetServer())->channel_exists(args.at(1)))
    {
        send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(1) + " :No such channel" + "\r\n"), client);
        return ;
    }
    channel *chann = client->GetServer()->find_channel(args.at(1));
    if (!chann)
        return ;
    if (!chann->is_member(client->getNickname()))
    {
        send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(1) + " :You're not on that channel" + "\r\n"), client);
        return ;
    }

    //second part
    if (args.size() == 2)
    {
        if ((chann->get_topic()).empty())
        {
            send_message((":" + host_name() + " 331 " + client->getNickname() + " " + args.at(1) + " :No topic is set" + "\r\n"),client);
            return ;
        }
        else
        {
            send_message((":" + host_name() + " 332 " + client->getNickname() + " " + args.at(1) + " " + chann->get_topic() + "\r\n"),client);
            return ;
        }
    }
    else if (args.size() > 2)
    {
        if (chann->is_topic_restricted())
        {
            if (!chann->is_mod(client->getNickname()))
            {
                send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(2) + " :You're not channel operator" + "\r\n"), client);
                return ;
            }
        }
        chann->change_topic(args.at(2));
        broadcast_message((":" + host_name() + " 332 " + client->getNickname() + " " + args.at(1) + " " + chann->get_topic() + "\r\n"), chann->all_clients() ,client);
    }
}

//still haven't included l flag
void    mode_commands(std::vector<std::string> args, Client *client)
{
    char    i = ' ';
    char    t = ' ';
    char    k = ' ';
    char    o = ' ';
    char    l = ' ';
    std::string flags;
    std::vector<std::string>::iterator it;
    channel                            *chann = NULL;
    
    if (args.size() < 3)
    {
        send_message((":" + host_name() + " 461 " + client->getNickname() + " MODE :Not enough parameters" + "\r\n"), client);
        return ;
    }
    chann = (client->GetServer())->find_channel(args.at(1));
    if (!chann)
        return ;
    if (!chann->is_mod(client->getNickname()))
    {
        send_message((":" + host_name() + " 482 " + client->getNickname() + " " + args.at(2) + " :You're not channel operator" + "\r\n"), client);
        return ;
    }
    for (it = args.begin(); it != args.end(); ++it)
    {
        if ((*it)[0] == '+' || (*it)[0] == '-')
        {
            for (size_t j = 1; j < (*it).length(); ++j)
            {
                char currentChar = (*it)[j];
                if (currentChar == 'i')
                    i = (*it)[0];
                if (currentChar == 't')
                    t = (*it)[0];
                if (currentChar == 'k')
                    k = (*it)[0];
                if (currentChar == 'o')
                    o = (*it)[0];
                if (currentChar == 'l')
                    l = (*it)[0];
            }
        }
    }
    //i==================
    if (i != ' ')
    {
        if (i == '+')
        {
            chann->set_inv_status(true);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +i" + "\r\n", client);
            std::cout << "invite only now" << std::endl;
        }
        else if (i == '-')
        {
            chann->set_inv_status(false);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -i" + "\r\n", client);
            std::cout << "not invite only now" << std::endl;

        }
    }
    //t===================
    if (t != ' ')
    {
        if (t == '+')
        {
            chann->set_topic_restriction(true);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +t" + "\r\n", client);

        }
        else if (t == '-')
        {
            chann->set_topic_restriction(false);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -t" + "\r\n", client);
        }
    }
    //k======================
    if (k != ' ')
    {
        if (k == '+')
        {
            chann->pw_restriction_status(true);
            if (args.size() >= 4)
            {
                chann->set_newpw(args.at(3));
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +k " + args.at(3) + "\r\n", client);
            }
        }
        else if (k == '-')
        {
            chann->pw_restriction_status(false);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -k" + "\r\n", client);
        }
    }
    //o=====================
    if (o != ' ')
    {
        Client *newc = NULL;

        if(args.size() >= 4)
        {
            newc = (client->GetServer())->find_user_bynick(args.at(2));
        }
        if (!newc)
            return ;
        if (o == '+')
        {
            if (chann->is_mod(newc->getNickname()))
                return ;
            chann->add_mod(newc);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +o " +"\r\n", client);
            send_message(":" + host_name() + " 324 " + newc->getNickname() + " " + args.at(1) + " +o " + "\r\n", newc);
        }
        else if (o == '-')
        {
            if (!(chann->is_mod(newc->getNickname())))
                return ;
            chann->remove_mod(newc);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -o " + "\r\n", client);
            send_message(":" + host_name() + " 324 " + newc->getNickname() + " " + args.at(1) + " -o " + "\r\n", newc);
        }
    }
    //l============================
    if (l != ' ')
    {
        if (l == '-')
        {
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -l " + args.at(3) + "\r\n", client);
            chann->set_size(100);
        }
        else if (l == '+')
        {
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +l " + args.at(3) + "\r\n", client);
            chann->set_size(std::atoi((args.at(3)).c_str()));
        }
    }
}

//other commands
void    privmsg_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() < 3)
    {
        send_message((":" + host_name() + " 412 " + client->getNickname() + " :No text to send" + "\r\n"), client);
        return ;
    }
    std::vector<std::string> tokens = splitString(args.at(1), ',');

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        args.at(1) = tokens[i];
        if ((args.at(1)).at(0) == '#')
        {
            if (!(client->GetServer())->channel_exists(args.at(1)))
            {
                send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(1) + " :No such channel" + "\r\n"), client);
                continue ;
            }
            channel *chann = (client->GetServer())->find_channel(args.at(1));
            if (!chann)
                continue ;
            if (chann->is_user_restricted())
            {
                send_message((":" + host_name() + " 404 " + client->getNickname() + " " + args.at(1) + " :Cannot send to channel!" + "\r\n"), client);
                continue ;
            }
            if (chann->is_member(client->getNickname()))
            {
                broadcast_message(":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + ".ip PRIVMSG " + args.at(1) + " " + trimPoints(args.at(2)) + "\r\n", chann->all_clients(), client);
                continue ;
            }
            else
            {
                send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(1) + " :You're not on that channel" + "\r\n"), client);
                continue ;
            }
        }
        if (!(client->GetServer())->client_exists(args.at(1)))
        {
            std::cout << "here -- -----    " << args.at(1) <<  std::endl;
            send_message((":" + host_name() + " 401 " + client->getNickname() + " " + args.at(1) + " :No such nick/channel" + "\r\n"), client);
            continue ;
        }
        else
        {
            Client *receiver = (client->GetServer())->find_user_bynick(args.at(1));
            std::string str = trimPoints(args.at(2));
            if (str[0] == ':')
                str = str.erase(0, 1);
            if (receiver)
                send_message(":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + ".ip PRIVMSG " + args.at(1) + " :" + str + "\r\n", receiver);
        }
    }
}

void    notice_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() < 3)
    {
        return ;
    }
    if ((args.at(1)).at(0) == '#')
    {
        if (!(client->GetServer())->channel_exists(args.at(1)))
        {
            return ;
        }
        channel *chann = (client->GetServer())->find_channel(args.at(1));
        if (!chann)
            return ;
        if (chann->is_user_restricted())
        {
            return ;
        }
        if (chann->is_member(client->getNickname()))
        {
            broadcast_message(":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + ".ip NOTICE " + args.at(1) + " " + trimPoints(args.at(2)) + "\r\n", chann->all_clients(), client);
            return ;
        }
        else
        {
            return ;
        }
    }
    if (!(client->GetServer())->client_exists(args.at(1)))
    {
        return ;
    }
    else
    {
        Client *receiver = (client->GetServer())->find_user_bynick(args.at(1));
        if (receiver)
            send_message((":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " NOTICE " + args.at(1) + " " + trimPoints(args.at(2)) + "\r\n"), receiver);
    }
}

void	quit_cmd(Client *client, std::vector<std::string> args)
{
    if (args.size() > 1)
        (client->GetServer())->broadcast_channels(client, (":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " QUIT :" + args.at(1) + "\r\n"));
    else
        (client->GetServer())->broadcast_channels(client, (":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " QUIT" + "\r\n"));
    (client->GetServer())->delete_client(client);
}

//ping command



