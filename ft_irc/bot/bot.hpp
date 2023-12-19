#include "../include/irc.hpp"


class Bot {

    public:
        Bot(std::string nickname, std::string password, std::string address, int port);
        ~Bot();

        void connectServ();
        void init();
        bool parseMessage(char *buffer);
        void botCommands(std::string cmd);


    private:    
        std::string nickname;
        std::string username;
        std::string hostname;
        std::string password;
        std::string address;

        std::string clientNick;
        int clientSocket;
        int socketLength;
        int port;
        struct sockaddr_in clientAddress;
        std::vector<std::string> quotes;
};

bool    validPort(std::string port);
bool    validPassword(std::string password);
bool    validNickname(std::string nickname);
bool    parseCommands(char** argv);
void    printHelp(std::string msg);
void	clearBuffer(char *buffer, int size);
std::string getTime();