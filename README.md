ft_irc - Internet Relay Chat

Table of Contents

    Introduction
    General Rules
    Mandatory Part
        Requirements
        For MacOS only
        Test Example
    Bonus Part
    Submission and Peer-evaluation

Introduction

The ft_irc project is an opportunity to create your own Internet Relay Chat (IRC) server. IRC is a text-based communication protocol that allows users to engage in real-time messaging, both publicly and privately. You can exchange direct messages and join group channels. IRC clients connect to IRC servers to participate in this network of communication.

This project challenges you to implement a functional IRC server using C++ 98. You will develop the server, while a reference IRC client will be used for testing. The project aims to familiarize you with internet standards and protocols, which is essential knowledge for working in networking and system administration fields.


General Rules

    Your program must be robust and not crash under any circumstances, even when running out of memory. Unexpected quits result in a grade of 0.
    You must provide a Makefile to compile your source files.
    The Makefile should include the following rules: $(NAME), all, clean, fclean, and re.
    Compile your code with c++ and the flags -Wall -Wextra -Werror.
    Your code must adhere to the C++ 98 standard.
    Prefer using C++ features over C functions whenever possible.
    External libraries and Boost libraries are not allowed.


Mandatory Part

Program Name: ircserv

Turn in files: Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp (and an optional configuration file)

Makefile Rules: NAME, all, clean, fclean, re



Arguments:

    port: The listening port
    password: The connection password



External Functions: Everything in C++ 98, including functions like socket, close, setsockopt, getsockname, getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo, bind, connect, listen, accept, htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa, send, recv, signal, sigaction, lseek, fstat, fcntl, poll (or equivalent)


Libft Authorized: N/A


Description: Implement an IRC server in C++ 98 that can handle multiple clients simultaneously. Forking is not allowed; all I/O operations must be non-blocking. Use only one poll() (or equivalent) for handling operations. Your server should be able to authenticate, set a nickname, a username, join a channel, send and receive private messages, and forward messages to all clients that joined a channel. Implement operators and regular users, as well as commands specific to channel operators.


Requirements

    The server must handle multiple clients simultaneously and never hang.
    You must use non-blocking file descriptors.
    Only one poll() (or equivalent) should be used for handling all operations.
    Implement communication between the client and server via TCP/IP (v4 or v6).
    Your reference client must connect to your server without errors.



For MacOS Only

For MacOS, use fcntl() to achieve non-blocking file descriptors with the following command:

fcntl(fd, F_SETFL, O_NONBLOCK);

*Any other flags are forbidden.


Test Example

Verify every possible error and issue, including receiving partial data and low bandwidth. To ensure that your server correctly processes everything, use a simple test with nc as follows:

sh-----------------------
                        |
$> nc 127.0.0.1 6667    |
com^Dman^Dd             |
$>                      |
-------------------------

Use Ctrl+D to send the command in several parts: 'com', then 'man', then 'd\n'. You need to aggregate received packets to rebuild the command.


Bonus Part

You can add extra features to your IRC server to make it more like an actual IRC server, such as handling file transfers or implementing a bot. The bonus part will only be assessed if the mandatory part is perfect, meaning all the mandatory requirements are met without any malfunctions.
