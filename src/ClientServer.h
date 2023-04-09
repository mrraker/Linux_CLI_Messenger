#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <wait.h>

class ClientServer // Declaring abstract class ClientServer
{

public: // Declaring abstract class ClientServer public attributes and functions

	struct sockaddr_in addr; // Structure that will contain information about socket/IP-Protocol interaction
	struct hostent* server; // Structure that is used to translate hostname/IP-address from argv[] to needed format
	ClientServer();	// Class constructor
	char buf[1024]; // Variable for buffer to save received messages by function recv()
	char message[1024];	//	Variable for inputing the meessage from CLI interface to function send()	 
	std::string messageStr; // string class for editing messages to be sent


};


class Client : public ClientServer	// Declaring class Client wich is a child of a class Client-Server
{
public:	// Declaring class Client public attributes and functions

	Client(int argc, char* argv[]);	// Class constructor
	~Client();	// Class destructor
private:	// Declaring class Client private attributes and functions

	int sock;	// integer variable for defining a socket (descriptor) (for connection, sending and receiving messages)
	signed int Connect(int sock);	// function initiating connection to the Server
	signed int SendMessage(int sock, fd_set readset);	//	Sending message function (To Server)
	signed int ReceiveMessage(int sock, fd_set readset);	// Receiving message function (From Server)

};


class Server : public ClientServer	// Declaring class Server wich is a child of a class Client-Server
{

public:	// Declaring class Server public attributes and functions

	Server(int argc, char* argv[]);	// Class constructor
	~Server();	// Class destructor

private: // Declaring class Client public attributes and functions

	int listener;	// integer variable for defining a listening socket (waiting for connection request from the Client)
	int connectedUsers = 0;	// Connected users counter 
	std::set<int> clients;	//	container that will contain the sockets for each Client


	signed int ConnectUsers(int listener, fd_set readset);	// function accepting connection requests from the Clients
	signed int SendMessage(fd_set readset);	// 	Sending message function (Broadcast)
	signed int ReceiveMessage(std::set<int>::iterator it, fd_set readset);	// Receiving and broadcasting Clients messages function 

};

