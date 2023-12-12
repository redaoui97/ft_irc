#include "../include/bot.hpp"

Bot::Bot() {
	std::cout << "Bot constructor called" << std::endl;
}

void	Bot::setQuote(const std::string& newQuote) {
	quotes.push_back(newQuote);
}

std::string	Bot::quote() const {
	size_t index;

	if(quotes.empty()) {
		normal_error("No Quotes Available");
		return ("");
	}
	index = std::rand() %quotes.size();
	return (quotes[index]);
}

Bot::~Bot() {
	std::cout << "Bot Destructor called" << std::endl;
}
