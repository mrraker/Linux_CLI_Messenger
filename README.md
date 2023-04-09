About
================================================================================================================================================================
Simple CLI chat-room (server and client) for Linux.

Building manual
================================================================================================================================================================
To build a executive files Server.out and Client.out on Unix unpack this .gzip archive to some directory and run the following command:
make -f Client-server_chat_new.mak. 

Client manual
================================================================================================================================================================
To start a Client - run the file Client.out with optional arguments "Server IP-address(HostName)" and "Port". Command example:
./Client.out 192.168.0.1 3430
./Client.out localhost 3430

If some arguments were missed in command line, the defaul vlaues will be as follows:
Server IP-address: 127.0.0.1 (localhost)
Port: 3425

After the Client has started and the connection to Server has been estblished successfuly you'll get a message on your CLI clipboard:
"Welcome to our chat-room!" and how many users are also connected to chat-Server.
If connection fails - Client will be shut down.
To send a message to the chat-room - type it in a CLI Clipboard and press "Enter".
To exit the chat-room and close Client type "exit" and press "Enter".

Server manual
================================================================================================================================================================
To start a Server run the file "Server.out" with arguments "Workmode" ("-d" for daemon mode or "-i" for interface mode) and "PortNumber"(TCP port number). 
Command example:
./Server.out -d 3430

If no arguments were added to command line, the default vlaues will be as follows:
Workmode: -i
PortNumber: 3425

Interface mode:
After the Server started you'll get a message "The chat-server has been successfuly started" on your CLI clipboard.
Now it can accept connection requests from Clients.
To send a message from Server to the chat-room - type it in a CLI Clipboard and press "Enter".
To disconnect all users and shut down the server type "exit" and press "Enter".

Daemon mode:
After the Server started you'll get a message "Server has been started is a daemon mode" on your CLI clipboard.
To disconnect all users and shut down the server run the file "Server.out" once more with "-c" argument. Example:
./Server.out -c




