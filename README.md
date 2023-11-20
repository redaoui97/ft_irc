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
Sockets are a way to speak to other programs using standard Unix <a href="https://www.informit.com/articles/article.aspx?p=174659&seqNum=8"> File Descriptors</a> and socket calls.  

#### Socket types
There are all kinds of sockets: DARPA internet addresses, path names on a local node (Unix Sockets), CCITT X.25 addresses... But the most important one that I'll be using is Internet Sockets.  

#### Internet sockets types
There are many types of Internet Sockets. But three of them matter to us:

> Raw Sockets
<ul>
    <li>Typically used for network diagnostic, network analysis and security applications for tasks like packet capturing, monitoring and crafting custom network packets. Not commonly used in regular applications</li>
    <li>Type: Low-level</li>
    <li>Various protocols, including IP (Internet Protocol)</li>
    <li>Characteristics:
        <ul>
            <li>Provides direct access to network protocol stack</li>
            <li>Allows crafting and sending custom packets at the network layer</li>
            <li>Offers more control but requires careful handling</li>
        </ul>
    </li>
</ul>

> Stream Sockets (SOCK_STREAM)
<ul>
    <li>Used when data integrity and order are critical</li>
    <li>Type: Connection-Oriented</li>
    <li>Protocol: TCP</li>
    <li>Characteristics: 
        <ul>
            <li>Ordered data transmission</li>
            <li>Full-duplex data transmission</li>
            <li>Guarantees data integrity</li>
            <li>Requires connection before data exchange</li>
        </ul>
    </li>
</ul>

> Datagram Sockets (SOCK_DGRAM)
<ul>
    <li>Used when speed is prioritized over data reliability</li>
    <li>Type: Connectionless</li>
    <li>Protocol: UDP</li>
    <li>Characteristics: 
        <ul>
            <li>Unordered data transmission</li>
            <li>Data is sent as discrete packets (datagrams) with no guaranteed delivery or order</li>
            <li>Faster but less reliable than Stream Sockets</li>
        </ul>
    </li>
</ul>

#### Network theory
During the data transmission over a network, the data is encapsulated, then is stripped off those layers when arrives to its destination.  
<a href="https://www.cloudflare.com/learning/ddos/glossary/open-systems-interconnection-model-osi/">OSI</a> is a network model that describes a system of network functionality.  
I'm not going to go through the protocols and the encapulation methods used during data transmission, and will instead put Socket under the microscope.  
For Sockets, all you have to do is call the send() sys-call for Stream Sockets. Or sendto() (some encapsulated data using a method). The Kernel builds the Transport Layer and Internet Layer, and the hardware does the Network Access Layer.  
Detailed documentation on networking <a href="https://datatracker.ietf.org/doc/html/rfc792">rfc791</a>.
### Notes from the Beej Sockets doc:
<ul>
    <li>ip addresses(switching from class notation into cidr notation<); subnets; port numbers; byte order(/li>
</ul>
