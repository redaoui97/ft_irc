# ft_irc

## Project structure:
-Mess up a bit with IRC servers, understand how they work : irc.libera.chat example irc server
-Socket vs webSocket? : https://github.com/vasanthk/how-web-works#opening-of-a-socket--tls-handshake
-Set project files; Makefile; Header
-Canonical project structure paper: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html
-Host irc server on Onion Router https://infosecwriteups.com/create-a-hidden-irc-server-with-the-onion-router-tor-c839e3a81d78
-Fully functional server from IBM : https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select
-IBM documentation for sockets : https://www.ibm.com/docs/en/i/7.1?topic=communications-socket-programming
## Network Programming:

### Reference
I'll be using Beej's Guide to Network Programming, it's based on the Berkley Socket API.

### Sockets
Sockets are a way to speak to other programs using standard Unix File Descriptors. 