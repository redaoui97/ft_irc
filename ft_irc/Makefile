NAME	= ircserv
CC		= c++
FLAGS	= -Iinclude -Wall -Wextra -Werror -std=c++98 
HEADER	= include/irc.hpp

MAIN	= $(addprefix srcs/, main server client channel errors parsing commands utils)
SRC		= $(MAIN:=.cpp)
OBJ_DIR = obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(notdir $(MAIN:=.o)))

.PHONY: all clean fclean re

all: $(NAME)
	
$(NAME): $(OBJ) $(HEADER)
	$(CC) $(FLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: srcs/%.cpp $(HEADER) | $(OBJ_DIR)
	$(CC) $(FLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all
