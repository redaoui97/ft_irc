#ifndef BOT_HPP
# define BOT_HPP

#include "../include/irc.hpp"

class Bot {

	public:
		Bot();
		~Bot();

		std::string	quote() const;
		void	setQuote(const std::string& newQuote);

	private:
		std::vector<std::string> quotes;
};

#endif
