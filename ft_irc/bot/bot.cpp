#include"bot.hpp"

Bot::Bot(std::string nickname, std::string password, std::string address, int port) {
    this->nickname = nickname;
    this->username = "BotUserName";
    this->password = password;
    this->address = address;
    this->port = port;
    init();
    printHelp("Bot created");
    connectServ();
    printHelp("Bot connected");
}

void Bot::init() {
    this->quotes = new std::vector<std::string>;
    this->quotes->push_back("Live the moment.");
    this->quotes->push_back("Find your rhythm.");
    this->quotes->push_back("Kindness speaks.");
    this->quotes->push_back("Stars need darkness.");
    this->quotes->push_back("Love your work.");

    this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->clientSocket == -1) {
        std::cout << "Error: socket creation failed" << std::endl;
        exit(1);
    }
    this->socketLength = sizeof(this->clientAddress);
    this->clientAddress = new struct sockaddr_in;
    this->clientAddress->sin_family = AF_INET;
    this->clientAddress->sin_port = htons(this->port);
    this->clientAddress->sin_addr.s_addr = inet_addr(this->address.c_str());
}
void Bot::connectServ() {

    if (connect(this->clientSocket, (struct sockaddr *)this->clientAddress, this->socketLength) == -1) {
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

    msg = "bot connected to server secssfully\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);
    while(1) {
        char buffer[1024];
        clearBuffer(buffer, 1024);
        if (recv(this->clientSocket, buffer, 1024, 0) < 0) {
            printHelp("Error: recv failed");
            exit(1);
        } else if (!parseMessage(buffer)) {
            printHelp("Error: invalid message");
            exit(1);
        } else {
            run();
        }
    }
}

void Bot::run() {
    std::size_t randomIndex = std::rand() % this->quotes->size();
    std::string msg = "PRIVMSG #test : " + this->quotes->at(randomIndex) + "\r\n";
    send(this->clientSocket, msg.c_str(), msg.length(), 0);
}

Bot::~Bot() {
    delete this->quotes;
    delete this->clientAddress;
    std::cout << "Bot destroyed" << std::endl;
}