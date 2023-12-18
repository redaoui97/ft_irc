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
    msg = "NICK " + this->nickname + "\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);
    msg = "USER " + this->username + " 0 * :" + this->username + "\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);

    std::cout << "Bot connected to server" << std::endl;
    while(1) {
        char buffer[1024];
        clearBuffer(buffer, 1024);
        if (recv(this->clientSocket, buffer, 1024, 0) < 0) {
            printHelp("Error: recv failed");
        } else {
            std::cout << "this is the buffer: " << buffer << std::endl;
            if (!parseMessage(buffer)) {
                printHelp("Error: invalid message");
            }
        }
    }
}



bool    Bot::parseMessage(char *buffer) {
    std::string msg(buffer);
    std::istringstream iss(msg);
    std::string cmd;

    iss >> cmd;
    iss >> cmd;
    std::cout << "Received cmd: " << cmd << std::endl;
    if (cmd == "PRIVMSG") {
        std::string botNick;
        std::string clientNickName;
        
        iss >> botNick;
        iss >> clientNickName;
        if (clientNickName[0] == ':') {
            if (clientNickName[1] == ':')
                clientNickName.erase(0, 2);
            else
                clientNickName.erase(0, 1); // check the data recieved from the server is in the form of ::<nickname> - extra :
        }
        clientNick = clientNickName;
        std::cout << "botNick: " << botNick << std::endl;
        std::cout << "clientNick: " << clientNickName << std::endl;
        std::size_t randomIndex = std::rand() % this->quotes.size();
        std::cout << this->quotes[randomIndex] << std::endl;
        std::string message = "PRIVMSG " + clientNickName + " :" + this->quotes[randomIndex];
        ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
        if (sentBytes == -1) {
            std::cout << "Error: sending message failed" << std::endl;
        }
    }
    return true;
}

Bot::~Bot() {
    std::cout << "Bot destroyed" << std::endl;
    std::string message = ":" + this->nickname + " PRIVMSG " + this->clientNick + " :Bot destroyed";
    ssize_t sentBytes = send(this->clientSocket, message.c_str(), message.length(), 0);
    if (sentBytes == -1) {
        std::cout << "Error: sending message failed" << std::endl;
    }    
}