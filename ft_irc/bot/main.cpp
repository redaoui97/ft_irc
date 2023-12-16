#include"bot.hpp"

int main   (int argc, char** argv) {

    if (argc != 5) {
        printHelp("Usage: ./bot <nickname> <password> <address> <port>");
        return 1;
    }
    if (!parseCommands(argv)) {
        return 1;
    }
    Bot bot(argv[1], argv[2], argv[3], atoi(argv[4]));
    bot.run();
    return 0;
}