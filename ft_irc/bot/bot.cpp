#include"bot.hpp"

Bot::Bot(std::string nickname, std::string password, std::string address, int port) {
    this->nickname = nickname;
    this->username = "BotUserName";
    this->hostname = "BotHostName";
    this->password = password;
    this->address = address;
    this->port = port;
    this->clientNick = "";
    init();
    printHelp("Bot created");
    connectServ();
    printHelp("Bot connected");
}

void Bot::init() {
    this->quotes.push_back("Live the moment.");
    this->quotes.push_back("Find your rhythm.");
    this->quotes.push_back("Kindness speaks.");
    this->quotes.push_back("Stars need darkness.");
    this->quotes.push_back("Love your work.");

    this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->clientSocket == -1) {
        std::cout << "Error: socket creation failed" << std::endl;
        exit(1);
    }
    this->socketLength = sizeof(this->clientAddress);
    this->clientAddress.sin_family = AF_INET;
    this->clientAddress.sin_port = htons(this->port);
    this->clientAddress.sin_addr.s_addr = inet_addr(this->address.c_str());
}
void Bot::connectServ() {

    if (connect(this->clientSocket, (struct sockaddr *)&this->clientAddress, this->socketLength) == -1) {
        std::cout << "Error: connection failed" << std::endl;
        exit(1);
    }
    std::cout << "Connecting to server" << std::endl;
    std::string msg = "PASS " + this->password + "\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);
    // check if password is correct
    char buffer[20];
    int n = recv(this->clientSocket, buffer, 20,0 );
    buffer[n] = '\0';
    std::string response(buffer);
    if (response != "correct password\r\n") {
        std::cout << "Error: incorrect password" << std::endl;
        exit(1);
    }
    //
    msg = "NICK " + this->nickname + "\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);
    msg = "USER " + this->username + " " + this->hostname + " BserverName :BrealName\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);

    std::cout << "Bot connected to server" << std::endl;
    while(1) {
        char buffer[1024];
        clearBuffer(buffer, 1024);
   
        if (recv(this->clientSocket, buffer, 1024, 0) <= 0) {
            printHelp("Error: recv failed");
            break;
        } else {
            if (!parseMessage(buffer)) {
                printHelp("Error: invalid message");
            }
        }
    }
}

void Bot::botCommands(std::string cmd)
{
    if (cmd == "!help") {
        std::string msg = " :Available commands : [!quote] [!8ball] [!time] \r\n";
        std::string message = "PRIVMSG " + this->clientNick + msg;
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    } else if (cmd == "!quote") {
        std::size_t randomIndex = std::rand() % this->quotes.size();
        std::string message = "PRIVMSG " + this->clientNick + " :" + this->quotes[randomIndex] + "\r\n";
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    } else if(cmd == "!8ball") {
        size_t randomIndex = std::rand() % 10;
        std::string msg = " :[!8ball] [Your lucky number is: " + std::to_string(randomIndex) + "]";
        std::string message = "PRIVMSG " + this->clientNick + msg  + "\r\n";
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    } else if(cmd == "!time") {
        std::string msg = getTime() ;
        std::string message = "PRIVMSG " + this->clientNick + " :" + msg + "\r\n";
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    } else {
        std::string message = "PRIVMSG " + this->clientNick + " :Invalid command, type [!help] for more info"  + "\r\n";
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    
    }
}

bool    Bot::parseMessage(char *buffer) {
    std::string msg(buffer);
    std::istringstream iss(msg);
    std::string cmd;
    std::string Bcmd;

    iss >> cmd;
    iss >> cmd;
    if (cmd == "PRIVMSG") {
        std::string botNick;
        std::string clientNickName;
        
        iss >> std::ws >> botNick >> std::ws >> clientNickName >> std::ws >> Bcmd;
        clientNick = clientNickName;
        while (clientNick[0] == ':')
            clientNick.erase(0, 1);
        std::string tmp;
        iss >> tmp;
        if (!tmp.empty()) {
            std::string message = "PRIVMSG " + clientNick + " :Invalid command, type [!help] for more info\r\n";
            ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
            if (sentBytes == -1 || sentBytes == 0) {
                std::cout << "Error: sending message failed" << std::endl;
            }
        } else {
            botCommands(Bcmd);
        }
    }
    return true;
}

Bot::~Bot() {
    std::cout << "Bot destroyed" << std::endl;
}