#include "client.hpp"

channel::channel(std::string name, Client *client, std::string password)
{
    if (password.empty())
        m_password_restrict = false;
    else
    {
        m_password_restrict = true;
        m_password = password;
    }
    m_name = name;
    m_topic= "";
    m_invite_only = false;
    m_topic_restricted = true;
    m_user_restricted = false;
    add_mod(client);
    add_client(client);
}

void channel::set_topic_restriction(bool status)
{
    m_topic_restricted = status;
}

void channel::set_user_restriction(bool status)
{
    m_user_restricted = status;
}

bool    channel::is_user_restricted()
{
    return (m_user_restricted);
}

void channel::remove_mod(Client *client)
{
    m_moderators.erase(client->getNickname());
}

bool    channel::is_mod(std::string nick)
{
    for (std::map<std::string, Client*>::const_iterator it = m_moderators.begin(); it != m_moderators.end(); ++it)
    {
        if (!(it->first).compare(nick))
        {
            std::cout << nick << " is mod!" << std::endl;
            return true;
        }
    }
    std::cout << nick << " is not a mod!" << std::endl;
    return false; 
}

void channel::pw_restriction_status(bool status)
{
    m_password_restrict = status;
}

void channel::set_newpw(std::string password)
{
    m_password = password;
}

bool    channel::require_pw()
{
    if (m_password_restrict)
        return (true);
    return (false);
}

bool    channel::require_invite()
{
    if (m_invite_only)
        return (true);
    return (false);
}

bool    channel::is_member(std::string nick)
{
    for (std::map<std::string, Client*>::const_iterator it = m_members.begin(); it != m_members.end(); ++it)
    {
        if (it->first == nick)
        {
            return true;
        }
    }
    return false; 
}

bool    channel::is_right_pw(std::string password)
{
    if (password.compare(m_password) == 0)
        return (true);
    return (false);
}
bool    channel::is_invited(std::string nick)
{
    for (std::vector<std::string>::const_iterator it = m_invited_members.begin(); it != m_invited_members.end(); ++it) {
        if (*it == nick)
        {
            return true; 
        }
    }
    return false;
}

void channel::set_inv_status(bool status)
{
    m_invite_only = status;
}
void channel::add_toinvite(Client *client)
{
    m_invited_members.push_back(client->getNickname());
}

void channel::change_topic(std::string topic)
{
    if (!topic.empty() && topic[0] == ':')
        m_topic = topic.substr(1); 
    else
        m_topic = topic;
}

std::string channel::get_topic()
{
    return (m_topic);
}

void channel::add_client(Client *client)
{
    m_members.insert(std::make_pair(client->getNickname(), client));
    std::cout << "client added: " << client->getNickname() << " on channel:" << this->m_name << " user count: " << m_members.size() << std::endl;
}

void channel::add_mod(Client *client)
{
    m_moderators.insert(std::make_pair(client->getNickname(), client));
}

void channel::delete_client(Client *client)
{
    m_members.erase(client->getNickname());
}
void channel::delete_mod(Client *client)
{
    m_moderators.erase(client->getNickname());
}

std::map<std::string, Client *> channel::all_clients()
{
    return (m_members);
}

std::string channel::print_all_client()
{
    std::string result;
    std::map<std::string, Client*>::const_iterator it;

    for (it = m_members.begin(); it != m_members.end(); ++it)
    {
        result += it->first + " ";
    }
    if (!result.empty())
    {
        result.erase(result.size() - 1);
    }
    return result;
}

bool    channel::is_topic_restricted()
{
    return (m_topic_restricted);
}

std::string    channel::get_name()
{
    return (m_name);
}

channel::~channel()
{
    std::cout << "Channel deleted!" << std::endl;
}