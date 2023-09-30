# ft_irc

## Project structure:
-Mess up a bit with IRC servers, understand how they work : irc.libera.chat example irc server
-Socket vs webSocket? : https://github.com/vasanthk/how-web-works#opening-of-a-socket--tls-handshake
-Set project files; Makefile; Header
-Canonical project structure paper: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html
-Host irc server on Onion Router https://infosecwriteups.com/create-a-hidden-irc-server-with-the-onion-router-tor-c839e3a81d78

## Network Programming:

### Reference
I'll be using Beej's Guide to Network Programming, it's based on the Berkley Socket API.

### Sockets
Sockets are a way to speak to other programs using standard Unix File Descriptors. 