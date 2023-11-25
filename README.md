ft_irc - Internet Relay Chat

Table of Contents

    Introduction
    General Rules
    Network theory
    Notes from the Beej Sockets doc:
    Building the server
    socket life cycle

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

#### Network theory
During the data transmission over a network, the data is encapsulated, then is stripped off those layers when arrives to its destination.  
<a href="https://www.cloudflare.com/learning/ddos/glossary/open-systems-interconnection-model-osi/">OSI</a> is a network model that describes a system of network functionality.  
I'm not going to go through the protocols and the encapulation methods used during data transmission, and will instead put Socket under the microscope.  
For Sockets, all you have to do is call the send() sys-call for Stream Sockets. Or sendto() (some encapsulated data using a method). The Kernel builds the Transport Layer and Internet Layer, and the hardware does the Network Access Layer.  
Detailed documentation on networking <a href="https://datatracker.ietf.org/doc/html/rfc792">rfc791</a>.
### Notes from the Beej Sockets doc:
<ul>
    <li>ip addresses(switching from class notation into cidr notation<); subnets; port numbers; byte order; structs(sockaddr_in; addrinfo; sockaddr_storage)</li>
    <li>SysCalls</li>
</ul>
### Building the server
#### socket life cycle
<ol>
    <li>Create a socket</li>
    <li>Bind the socket to an addres</li>
    <li></li>
</ol>

