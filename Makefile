CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98
TARGET = ircserv
SRCS = main.cpp ./src/server.cpp ./src/client.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = ./include/server.hpp ./include/client.hpp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

fclean: clean

re: fclean all

