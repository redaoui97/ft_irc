#ifndef IRC_HPP
# define IRC_HPP

//don't add unecessary headers
#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <cstdlib> 
#include <cerrno>
#include <cctype>
#include <queue>
#include <sstream>
#include <map>
#include <netdb.h>
#include <time.h>

#define SERVER_NAME "super_duoIRC Network"

enum rpl_replies
{
	RPL_NONE = 300,
	RPL_USERHOST = 302,
	RPL_ISON = 303,
	RPL_AWAY = 301,
	RPL_UNAWAY = 305,
	RPL_NOWAWAY = 306,
	RPL_WHOISUSER = 311,
	RPL_WHOISSERVER = 312,
	RPL_WHOISOPERATOR = 313,
	RPL_WHOISIDLE = 317,
	RPL_ENDOFWHOIS = 318,
	RPL_WHOISCHANNELS = 319,
	RPL_WHOWASUSER = 314,
	RPL_ENDOFWHOWAS = 369,
	RPL_LISTSTART = 321,
	RPL_LIST = 322,
	RPL_LISTEND = 323,
	RPL_CHANNELMODEIS = 324,
	RPL_CREATIONTIME = 329,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_INVITING = 341,
	RPL_SUMMONING = 342,
	RPL_VERSION = 351,
	RPL_WHOREPLY = 352,
	RPL_ENDOFWHO = 315,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	RPL_LINKS = 364,
	RPL_ENDOFLINKS = 365,
	RPL_BANLIST = 367,
	RPL_ENDOFBANLIST = 368,
	RPL_INFO = 371,
	RPL_ENDOFINFO = 374,
	RPL_MOTDSTART = 375,
	RPL_MOTD = 372,
	RPL_ENDOFMOTD = 376,
	RPL_YOUREOPER = 381,
	RPL_REHASHING = 382,
	RPL_TIME = 391,
	RPL_USERSSTART = 392,
	RPL_USERS = 393,
	RPL_ENDOFUSERS = 394,
	RPL_NOUSERS = 395,
	RPL_TRACELINK = 200,
	RPL_TRACECONNECTING = 201,
	RPL_TRACEHANDSHAKE = 202,
	RPL_TRACEUNKNOWN = 203,
	RPL_TRACEOPERATOR = 204,
	RPL_TRACEUSER = 205,
	RPL_TRACESERVER = 206,
	RPL_TRACENEWTYPE = 208,
	RPL_TRACELOG = 261,
	RPL_STATSLINKINFO = 211,
	RPL_STATSCOMMANDS = 212,
	RPL_STATSCLINE = 213,
	RPL_STATSNLINE = 214,
	RPL_STATSILINE = 215,
	RPL_STATSKLINE = 216,
	RPL_STATSYLINE = 218,
	RPL_ENDOFSTATS = 219,
	RPL_STATSLLINE = 241,
	RPL_STATSUPTIME = 242,
	RPL_STATSOLINE = 243,
	RPL_STATSHLINE = 244,
	RPL_UMODEIS = 221,
	RPL_LUSERCLIENT = 251,
	RPL_LUSEROP = 252,
	RPL_LUSERUNKNOWN = 253,
	RPL_LUSERCHANNELS = 254,
	RPL_LUSERME = 255,
	RPL_ADMINME = 256,
	RPL_ADMINLOC1 = 257,
	RPL_ADMINLOC2 = 258,
	RPL_ADMINEMAIL = 259
};

enum err_replies
{
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHSERVER = 402,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_TOOMANYCHANNELS = 405,
	ERR_WASNOSUCHNICK = 406,
	ERR_TOOMANYTARGETS = 407,
	ERR_NOORIGIN = 409,
	ERR_NORECIPIENT = 411,
	ERR_NOTEXTTOSEND = 412,
	ERR_NOTOPLEVEL = 413,
	ERR_WILDTOPLEVEL = 414,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NOMOTD = 422,
	ERR_NOADMININFO = 423,
	ERR_FILEERROR = 424,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NICKCOLLISION = 436,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOLOGIN = 444,
	ERR_SUMMONDISABLED = 445,
	ERR_USERSDISABLED = 446,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_NOPERMFORHOST = 463,
	ERR_PASSWDMISMATCH = 464,
	ERR_YOUREBANNEDCREEP = 465,
	ERR_KEYSET = 467,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BANNEDFROMCHAN = 474,
	ERR_BADCHANNELKEY = 475,
	ERR_NOPRIVILEGES = 481,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_CANTKILLSERVER = 483,
	ERR_NOOPERHOST = 491,
	ERR_UMODEUNKNOWNFLAG = 501,
	ERR_USERSDONTMATCH = 502
};

class Client;
class channel;
//prototypes for orphan functions
	//error functions
void	fatal_error(std::string err_msg);
void	normal_error(std::string err_msg);
void	client_error(std::string err_msg);
	//parsing functions
void	parse(const std::string& port,const std::string& password);
int		parse_command(std::string command);
bool	valid_nickname(std::string nick);

//IRC command functions
	//processing input functions
void	authentication(std::vector<std::string>args, Client* client, std::string password);
void	pass_cmd(Client *client, std::vector<std::string> args, std::string password);
void 	user_cmd(Client *client, std::vector<std::string> args);
void 	nick_cmd(Client *client, std::vector<std::string> args);
void    join_cmd(Client *client, std::vector<std::string> args);
void 	privmsg_cmd(Client *client, std::vector<std::string> args);
void	quit_cmd(Client *client,  std::vector<std::string> args);
void 	trigger_welcome(Client *client);
void    mod_commands(std::vector<std::string> args, Client *client);
void    kick_commands(std::vector<std::string> args, Client *client);
void    invite_commands(std::vector<std::string> args, Client *client);
void    topic_commands(std::vector<std::string> args, Client *client);
void    mode_commands(std::vector<std::string> args, Client *client);

//util functions
std::string	to_String(int n);
std::string host_name();
std::string get_date();
void		clear_buffer(char *buffer, int size);
void 		send_message(std::string msg, Client *client);
void 		broadcast_message(std::string msg, std::map<std::string, Client *> clients);
void    	send_err(Client *client,  err_replies rep, std::string additional_message);
std::string trimSpaces(const std::string& str);
std::string trimPoints(const std::string& str);
#endif
