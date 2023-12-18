NAME	= ircbot
CC		= c++
FLAGS	= -Iinclude -Wall -Wextra -Werror -std=c++98 -g
HEADER	= bot.hpp

MAIN	= $(addprefix , main bot utils)
SRC		= $(MAIN:=.cpp)
OBJ_DIR = obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(notdir $(MAIN:=.o)))

.PHONY: all clean fclean re

all: $(NAME)
	
$(NAME): $(OBJ) $(HEADER)
	$(CC) -g $(FLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: %.cpp $(HEADER) | $(OBJ_DIR)
	$(CC) -g $(FLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all
