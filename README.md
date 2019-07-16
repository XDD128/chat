Overview: This is a chat program written for an Intro to Networks class for Cal Poly, CPE464. Clients, using the cclient program, can
forward messages to each other through the server program, called server. Supports message broadcast, multicast, unicast, and listing
of current users' handles.

Usage:
cclient: cclient handle server-name server-port
server: server [port-number]
(port-number is optional for server, will automatically be assigned if not present in command)

Commands in cclient:
Unicast/Multicast Message (To one or more users): %M num-handles destination-handle [destination-handle] [text]  
Broadcast Message (To all users): %B [text]
List all users: %L
Exit: %E
