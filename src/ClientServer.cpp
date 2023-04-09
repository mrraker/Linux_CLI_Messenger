#include <ClientServer.h>

ClientServer::ClientServer() // Defining abstract class ClientServer constructor (empty constructor)
{
}

Client::Client(int argc, char* argv[])	// Defining class Client constructor
{


	if (argc == 3) // check whether 3 arguments were input
	{
		addr.sin_port = htons(atoi(argv[2]));	// set port input from CLI
		server = gethostbyname(argv[1]);	// getting IP address from host name
		bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);	// applying IP-address to socket/IP-Protocol 
	}																					//	interaction structure
	else if (argc == 2) // check whether 2 arguments were input
	{
		server = gethostbyname(argv[1]);	// getting IP address from host name
		bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);	// applying IP-address to socket/IP-Protocol 
	}																					// interaction structure
	else
	{
		addr.sin_port = htons(3425);	// default port 3425
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	// Server is located on localhost
	}

	sock = socket(AF_INET, SOCK_STREAM, 0); // defining a socket (descriptor) for connect or send/receive messages to(from) Server with default
												//	values (socket - Internet connection, stream intercation, no additional flags
	addr.sin_family = AF_INET;	// filing the socket/IP-protocol iteraction structure by dafault values (socket - Internet domain connection)



	if (Connect(sock) >= 0) // check is socket is allocated properly and connection is successfuly established 
							// - then starting the Client works (otherwise - close programm)
	{

		while (1)	// infinite loop - programm works until the "exit" command
		{
			fd_set readset;			// declaring the descriptors read set 
			FD_ZERO(&readset);		// Clearing the read set
			FD_SET(sock, &readset);	// adding new  socket to read set
			FD_SET(0, &readset);	//	addin the CLI Input interdace to read set to get the typed message from user
			if (select(5, &readset, NULL, NULL, NULL) <= 0)	// select fuction which is monitoring the activity of sockets
			{
				perror("select");	// output an error in case of select funtion fail
				exit(3);			// and closing programm
			}

			if (SendMessage(sock, readset) < 0)	break;	// activating send message fuction if it is working properly, otherwise - closing programm
			if (ReceiveMessage(sock, readset) < 0)	break; // activating receiving message fuction if it is working properly, otherwise - closing programm
		}

	}

}

Client::~Client() // Defining class Client destructor (closing Client)
{
	close(sock); // closing socket
	std::cout << "\nSee you soon!\n";	//output a "Goodbuy" message in CLI
}



Server::Server(int argc, char* argv[])	// Defining class Server constructor
{
	if (argc == 3) addr.sin_port = htons(atoi(argv[2]));	// set port input from CLI
	else
		addr.sin_port = htons(3425); // default port 3425


	listener = socket(AF_INET, SOCK_STREAM, 0);	//defining a socket(descriptor) for listen for Clients	connection requests 
	addr.sin_family = AF_INET;	// filing the socket/IP-protocol iteraction structure by dafault values (socket - Internet domain connection)
	addr.sin_addr.s_addr = INADDR_ANY; // fiiling the IP-addres of a peer Client - all IP adresses


	if (listener < 0)	// check whether listener socket is allocated properly
	{
		perror("socket");	// if allocation failed output an error-message in CLI interface
		exit(1); // closing programm
	}

	fcntl(listener, F_SETFL, O_NONBLOCK); // seting a Non-blocking mode for the listener socket to allow it works in background mode


	if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0)	//	check whether bind (binding socket to a particular address)
																	//	fuction is working properly
	{
		perror("bind"); // in bind failes output a message in CLI Interface
		exit(2); //	closing programm
	}

	listen(listener, 2);	// start listen throgh the listening socket

	clients.clear();	// clear the user container


	std::cout << "\nThe chat-server has been successfuly started\n"; // output a "Server started" message to CLI interfece

	while (1)	// infinite loop - programm works until the "exit" or "-c" command
	{	

		fd_set readset;				// declaring the descriptors read set 
		FD_ZERO(&readset);			// Clearing the read set
		FD_SET(listener, &readset);	// adding new  socket to read set
		FD_SET(0, &readset);		//	addin the CLI Input interface to read set to get the typed message from user


		for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++) //adding all Clients sockets (descriptors) to read set
			FD_SET(*it, &readset);


		int mx = std::max(listener, *max_element(clients.begin(), clients.end())); //calculating the max value for all descriptors + 1 for select function
		if (select(mx + 1, &readset, NULL, NULL, NULL) <= 0)	// select fuction which is monitoring the activity of sockets
		{
			perror("select");	// output an error in case of select funtion fail
			exit(3);			// and closing programm
		}

		if (ConnectUsers(listener, readset) < 0) std::cout << "\nSocket allocation failed.\n";	// check whether the client is connected properly 


		for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)// receving messages from each Client in client container
		{																			// and broadcasting them

			if (ReceiveMessage(it, readset) < 0) break; // calling a receving message fuction
			
			//std::cout << "\nAfterclosingclientsbegin=" << *clients.begin() << "\n";
		}

		
		if (SendMessage(readset) < 0)	break; // send the broadcast message from Server until the "exit" command, in this case - close programm
		
	}

}




Server::~Server() // Class Server destructor
{
	std::cout << "\nServer has been stopped.\n";	// Output a message that programm is closing
}



signed int Client::Connect(int sock)	// Declaring the class Client fuction Connect 
{

	if (sock < 0)	// checking whether the socket is allocated properly
	{
		perror("socket");							// if allocation failed 
		std::cout << "Socket allocation failed..."; // output an error-message in CLI interface
		return -1;									// and return -1 value
	}


	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)	// check whether connetion is established propely
	{
		perror("connect");														// if connection failed 
		std::cout << "No connection to the server, programm is now closing...";	//output an error message in CLI interface
		return -1;																// and return -1 value
	}

	fcntl(sock, F_SETFL, O_NONBLOCK);	// set the socket as a Non-blocking one to allow background receving messages

	return 0; // if fucntion does ok - return 0 value
}


signed int Client::SendMessage(int sock, fd_set readset)	// Declaring the class Client fuction SendMessage
{

	if (FD_ISSET(0, &readset))	// check whether the CLI input descriptor (0) is active 
	{
		read(0, message, sizeof(message));	// getting the input message to "message" variable

		if (!strcmp(message, "exit\n"))	// if the message is "exit\n"
		{	
			send(sock, message, sizeof(message), 0); // send a message to Server through the socket
			return -1;					// return -1 value (close programm)
		}

		send(sock, message, sizeof(message), 0); // send a message to Server through the socket

		for (int i = 0; i < 1024; i++)
		{
			message[i] = 0;				// Clearing message array
		}
	}
	return 0;	// if fuction does ok - return 0 value
}

signed int Client::ReceiveMessage(int sock, fd_set readset)	// Declaring the class Client fuction ReceiveMessage
{
	
	if (FD_ISSET(sock, &readset))	// check whether the socket (Server connected) is active
	{
		int bytes = recv(sock, buf, sizeof(buf), 0);	//receive the message from Server and put it in a buf variable

		if (bytes <= 0)	// check whether the recv function failed
		{
			std::cout << "\nServer has terminated connetion.";	// in case of recv fuction fault (Server dissconected) output am error message
			return -1;											// and return a -1 value;
		}

		buf[bytes] = 0;	// marking the end of a message by a 0-flag
		std::cout << "\n" << buf; // output the reveived message in CLI interface
		for (int i = 0; i < 1024; i++)
		{
			buf[i] = 0;	// Clear the buffer 
		}
	}
	return 0; // if fuction does ok - return 0 value
}



signed int Server::ConnectUsers(int listener, fd_set readset)	// Declaring the class Server fuction ConnectUsers
{

	if (FD_ISSET(listener, &readset) && connectedUsers <= 10)	// check whether the listener socket is active (new connection request)
	{
		int sock = accept(listener, NULL, NULL);	// declaring a new socket (descriptor) and accept connection
		
		if (sock < 0)	// checking whether the socket is allocated properly
		{
			perror("accept");	// if allocation failed output an error-message in CLI interface
			return -1;			// and return -1 value
		}

		connectedUsers++; // increment connected users counter
		messageStr = "\nWelcome to our chat-room!\nNow connected " + std::to_string(connectedUsers) + " user(s).\nInput your message and press 'Enter' to send it.\nTo exit the chat-room and close the programm type 'exit' and press 'Enter' \n";
		// Editing a "user joined the chat" message
		send(sock, messageStr.c_str(), sizeof(message), 0);	//	send edited message

		messageStr.clear();	// clear the message string 

		fcntl(sock, F_SETFL, O_NONBLOCK);	// setting new socket to a Nonblocking mode (to allow background receving messages)



		clients.insert(sock);	//	adding a socket to a clients container

		//for (std::set<int>::iterator itr = clients.begin(); itr != clients.end(); itr++) {
		//	std::cout << "\n" << *itr << "\n";
		//}
	}
	return 0; // if fuction does ok - return 0 value
}

signed int Server::SendMessage(fd_set readset)	// Declaring the class Server fuction SendMessage
{
	
	if (FD_ISSET(0, &readset))	// check whether the CLI input descriptor (0) is active 
	{
		
		read(0, message, sizeof(message));	// getting the input message to "message" variable
		
		if (!strcmp(message, "exit\n"))		// if the message is "exit\n"
		{
			
			for (std::set<int>::iterator it2 = clients.begin(); it2 != clients.end(); it2++) close(*it2);// closing all User sockets (connections)
			clients.erase(clients.begin(), clients.end());		// erasing all cients from the container
			connectedUsers = 0;	//setting User counter to 0
			close(listener); // closing listener socket
			return -1;	//return a -1 value 
		}
		
		messageStr = "Server: " + std::string(message);	// editing the ending message
		std::cout << "\n" << messageStr << "\n"; // output the sending message

		for (std::set<int>::iterator it2 = clients.begin(); it2 != clients.end(); it2++)
		{

			send(*it2, messageStr.c_str(), sizeof(message), 0); // send message to all Clients

		}
		messageStr.clear();	// clear the message string

		for (int i = 0; i < 1024; i++)
		{
			message[i] = 0;	// clear the message char array
		}
	}

	return 0;	// if fuction does ok - return 0 value
}

signed int Server::ReceiveMessage(std::set<int>::iterator it, fd_set readset)	// Declaring the class Server fuction ReceiveMessage
{
	
	
	if (FD_ISSET(*it, &readset))	// check whether the socket (Server connected) is active
	{
	
		int bytes_read = recv(*it, buf, 1024, 0);	//receive the message from Client and put it in a buf variable

		if (bytes_read <= 0 || !strcmp(buf, "exit\n")) // check whther the Client sent an "exit\n" message
		{ 
			//std::cout << "\nit=" << *it << "\n";


			messageStr = "User_" + std::to_string(*it) + " has left the chat-room"; //edit a "User left the chat" message
			std::cout << "\n" << messageStr;										// output message to CLI interface
			for (std::set<int>::iterator it2 = clients.begin(); it2 != clients.end(); it2++)
			{
				//std::cout << "\nit2=" << *it2 << "\n";
				if (it2 != it)
				{
					send(*it2, messageStr.c_str(), 1024, 0);	// send  "User left the chat" message to all other Clients
				}
			
			}

			close(*it);			// close the disconnected socket;
			clients.erase(*it);	// Delete the disconnected client from clients container
			messageStr.clear();	// Clear the message string;
			connectedUsers--;	//	decrment the User counter
			return -1; // return -1 value;
		}


		messageStr = "User_" + std::to_string(*it) + ": " + std::string(buf); // edit a message from User (concatenate user number to message)

		std::cout << "\n" << messageStr << "\n"; // output User message in CLI interface

		for (std::set<int>::iterator it2 = clients.begin(); it2 != clients.end(); it2++)
		{	
			

			if (it2 != it)
			{	
				send(*it2, messageStr.c_str(), bytes_read, 0); // broadcast message to all other users
			}
		}
		messageStr.clear(); // clear message string 
		for (int i = 0; i < 1024; i++)
		{
			buf[i] = 0;	// clear the buf char array
		}

	}

	return 0;	// if fuction does ok - return 0 value
}


