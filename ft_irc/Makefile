NAME	= ircserv
CC		= c++
FLAGS	= -Iinclude -Wall -Wextra -Werror -std=c++98 
HEADER	= include/irc.hpp

MAIN	= $(addprefix srcs/, main server client)
SRC		= $(MAIN:=.cpp)
OBJ		= $(MAIN:=.o)

.PHONY: all clean fclean re

all: $(NAME)
	
$(NAME): $(OBJ) $(HEADER)
	$(CC) $(FLAGS) $(OBJ) -o $@

%.o: %.cpp $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all