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
        channel *chann = (client->GetServer())->find_channel(args.at(1));
        if (!chann)
            return ;
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
    channel *chann = client->GetServer()->find_channel(args.at(1));
    std::string statuss = " = ";
    std::string clients_connected = client->getNickname();
    if (chann)
    {
        broadcast_message((":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " JOIN " + args.at(1) + "\r\n"), chann->all_clients(), client);
        statuss = (chann->require_pw() ? " @" : " = ");
        clients_connected = chann->print_all_client();
    }
    send_message((":" + host_name() + " 353 " + client->getNickname() + statuss + args.at(1) + " :@" + clients_connected + "\r\n"), client);
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
    channel *chann = client->GetServer()->find_channel(args.at(2));
    if (!chann)
        return ;
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

void    kick_commands(std::vector<std::string> args, Client *client)
{
    if (args.size() < 3 || (args.at(1)).empty() || (args.at(2)).empty())
    {
        send_message(":" + host_name() +  " 461 " + client->getNickname() + " KICK :Not enough parameters" + "\r\n", client);
        return ;
    }
    Client *clt = (client->GetServer())->find_user_bynick(args.at(2));
    if (!clt)
        return ;
    channel *chann = (client->GetServer())->find_channel(args.at(1));
    if (!chann)
        return ;
    if (chann->is_mod(clt->getNickname()))
        return ;
    send_message((":" +  host_name() + " 312 " + client->getNickname() + args.at(1) + " " + args.at(2) + " Kicked by operator" +"\r\n"), client);
    chann->delete_client(clt);
    if (chann->is_mod(clt->getNickname()))
        chann->remove_mod(clt);
    send_message((":" + host_name() + " 312 " + client->getNickname() + " " + args.at(1) + " " + clt->getNickname() + " :You were kicked by operator" + "\r\n"), clt);
}

void    topic_commands(std::vector<std::string> args, Client *client)
{
    channel *chann = client->GetServer()->find_channel(args.at(1));

    if (args.size() == 2)
    {
        send_message((":" + host_name() + " 332 " + client->getNickname() + " " + args.at(1) + " Topic:" + chann->get_topic() + "\r\n"),client);
    }
    else if (args.size() == 3)
    {
        chann->change_topic(args.at(2));
        send_message((":" + host_name() + " 332 " + client->getNickname() + " " + args.at(1) + " Topic:" + chann->get_topic() + "\r\n"),client);
    }
}

void    mode_commands(std::vector<std::string> args, Client *client)
{
    char    i = ' ';
    char    t = ' ';
    char    k = ' ';
    char    o = ' ';
    int     pf = 0;
    std::string flags;
    std::vector<std::string>::iterator it;
    channel                            *chann = NULL;
    
    //this gives a segfault
    if (client)
        chann = (client->GetServer())->find_channel(args.at(1));
    if (chann)
        return ;
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
            {
                k = (*it)[0];
                if ((*it)[0] == '+')
                {
                    if (o != '+')
                        pf = 1;
                    else
                        pf = 2;
                }
            }
            if (currentChar == 'o')
                o = (*it)[0];
        }
    }
    if (i != ' ')
    {
        if (i == '+')
        {
            chann->set_inv_status(true);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +i" + "\r\n", client);
        }
        else if (i == '-')
        {
            chann->set_inv_status(false);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -i" + "\r\n", client);
        }
    }
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
    if (k != ' ')
    {
        if (k == '+')
        {
            chann->pw_restriction_status(true);
            if (pf == 1 && args.size() >= 4)
            {
                chann->set_newpw(args.at(4));
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +k " + args.at(4) + "\r\n", client);
            }
            else if (pf == 2 && args.size() >= 5)
            {
                chann->set_newpw(args.at(5));    
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +k " + args.at(5) + "\r\n", client);
            }
        }
        else if (k == '-')
        {
            chann->pw_restriction_status(false);
            send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -k" + "\r\n", client);
        }
    }
    if (o != ' ')
    {
        Client *newc = NULL;

        if(pf == 2 && args.size() >= 4)
        {
            newc = (client->GetServer())->find_user_bynick(args.at(4));
        }
        if (pf == 1 && args.size() >= 5)
        {
            newc = (client->GetServer())->find_user_bynick(args.at(5));
        }
        if (!newc)
            return ;
        if (o == '+')
        {
            if (chann->is_mod(newc->getNickname()))
                return ;
            chann->add_mod(newc);
            if (pf == 2)
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +o " + args.at(4) +"\r\n", client);
            if (pf == 1)
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " +o " + args.at(5) +"\r\n", client);
        }
        else if (o == '-')
        {
            if (!(chann->is_mod(newc->getNickname())))
                return ;
            chann->remove_mod(newc);
            if (pf == 2)
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -o " + args.at(4) +"\r\n", client);
            if (pf == 1)
                send_message(":" + host_name() + " 324 " + client->getNickname() + " " + args.at(1) + " -o " + args.at(5) +"\r\n", client);
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
    if ((args.at(1)).at(0) == '#')
    {
        if (!(client->GetServer())->channel_exists(args.at(1)))
        {
            send_message((":" + host_name() + " 403 " + client->getNickname() + " " + args.at(1) + " :No such channel" + "\r\n"), client);
            return ;
        }
        channel *chann = (client->GetServer())->find_channel(args.at(1));
        if (!chann)
            return ;
        if (chann->is_user_restricted())
        {
            send_message((":" + host_name() + " 404 " + client->getNickname() + " " + args.at(1) + " :Cannot send to channel!" + "\r\n"), client);
            return ;
        }
        if (chann->is_member(client->getNickname()))
        {
            broadcast_message(":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp() + ".ip PRIVMSG " + args.at(1) + " " + trimPoints(args.at(2)) + "\r\n", chann->all_clients(), client);
            return ;
        }
        else
        {
            send_message((":" + host_name() + " 442 " + client->getNickname() + " " + args.at(1) + " :You're not on that channel" + "\r\n"), client);
            return ;
        }
    }
    if (!(client->GetServer())->client_exists(args.at(1)))
    {
        send_message((":" + host_name() + " 401 " + client->getNickname() + " " + args.at(1) + " :No such nick/channel" + "\r\n"), client);
        return ;
    }
    else
    {
        Client *receiver = (client->GetServer())->find_user_bynick(args.at(1));
        if (receiver)
            send_message((":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PRIVMSG " + args.at(1) + " " + trimPoints(args.at(2)) + "\r\n"), receiver);
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
