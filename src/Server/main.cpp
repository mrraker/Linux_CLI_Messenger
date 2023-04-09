#include "ClientServer.h"


int main(int argc, char* argv[]) // declaring main fuction of Server
{


	if (argc > 3) // check whether user tried to input too many arguments
	{				//	and
		std::cout << "\nToo many arguments\n"; // output a message to CLI clipboard
		exit(-1);	// close programm with code "-1"
	}

	if (argc == 1) // check whether user input no arguments (starting server with default parameters
	{
		std::cout << "\nPlease choose '-i' for interface mode or '-d' for daemon mode\n"; // output a message to CLI clipboard
	}
	else if (argc > 1) // check whether user input from 2 to 3 arguments
	{
		if (strcmp(argv[1], "-d") == 0) // if first argument is "-d" starting server in daemon mode
		{	

			int fd;
			fd = open("/var/run/Server.pid", O_RDONLY); // checking "CIDServer.cid" with a reading mode

			if (fd) // if file is successfully opened
			{
				pid_t pid; // declaring a PID variable
				int len;	// declaring integer variable for openning file and read bytes lenght
				char pid_buf[16]; // declaring char buffer for reading PID
				len = read(fd, pid_buf, 16); // reading PID to "pid_buf" and count read bytes in "len" variable from "fd" file 
				pid_buf[len] = 0;  // input a 0-byte to buffer char array.
				pid = atoi(pid_buf); // converting char read from file to int

				if (pid != 0)
				{
					
					if (kill(pid, 0) == 0)
					{	

						std::string statFileName = "/proc/";
						statFileName += pid_buf;
						statFileName += "/stat";
						std::cout << "\n" << statFileName <<"\n";

						std::ifstream statFileifsteam(statFileName);

						std::string line;
						std::getline(statFileifsteam, line);

						std::string::size_type posOpenBraceEnd = line.find("(");
						std::string::size_type posCloseBraceEnd = line.find(")");

						std::string processName(line.erase(posCloseBraceEnd));
						processName = processName.substr(posOpenBraceEnd + 1);

						std::cout << "\n" << processName << "\n";

						if (processName == "Server.out")
						{
							std::cout << "\nAnother copy of a server is already running. PID is " << pid << ".\nPlease close it and try again\n";	// output a message to CLI clipboard
							exit(-1); // close programm with code "-1"
						}


					}
					else
					{
						if (remove("/var/run/Server.pid") != 0) std::cout << "\nCannot remove CID file!!!"; // delete the PID file
					}
					
				}
				
			}


			pid_t parpid;			// declaring variable for child-proccess PID
			parpid = fork();		// using a fork function to create a child-proccess (copy of the parent one) and get the PID of a child proccess 

			if (parpid < 0)	// if fork() returnes "-1" it means that child creation has been NOT successful
			{

				std::cout << "\nServer start has been failed\n"; //  output a message to CLI clipboard
				exit(1); // close programm with code "1"

			}
			else if (parpid != 0) // if fork() returned the PID of a child-proccess, it means that it was successfully created
			{
				std::cout << "\nServer has been started in a daemon mode\n"; // output a message to CLI clipboard
				exit(0); // close programm (parent proccess) with code "0"
			}

			setsid(); // getting child-proccess to a new session

			


			FILE* f; //declaring a file that will contain PID of a child proccess (to have opportunity to close it when it needed)

			f = fopen("/var/run/Server.pid", "w+"); // creating a file "CIDServer.cid" with a writing mode
			if (f) // if file is successfully created
			{
				fprintf(f, "%u", getpid()); // writing PID of a child-proccess to file
				fclose(f); // closing file
			}
			else
			{
				std::cout << "\nCannot open file\n\nServer now stopped"; // output a message to CLI clipboard
				exit(-1);	// close programm with code "-1"
			}

			close(STDIN_FILENO); // closing input descriptor
			close(STDOUT_FILENO); // closing output descriptor
			close(STDERR_FILENO); // closing error-output descriptor

			Server newServer = Server(argc, argv);					 // call the Server constructor
		}

		else if (strcmp(argv[1], "-i") == 0) // if user typed an "-i" argumet
		{
			std::cout << "\nServer has been started in a interface mode\n"; // output a message to CLI clipboard

			Server newServer = Server(argc, argv);					 // call the Server constructor (interface mode)
		}
		else if (strcmp(argv[1], "-c") == 0) // if user typed an "-c" argumet
		{
			FILE* f;	//declaring a file that contains PID of a child proccess
			pid_t pid; // declaring a PID variable

			int fd, len;	// declaring integer variable for openning file and read bytes lenght

			if (fd = open("/var/run/Server.pid", O_RDONLY)) // opening file "CIDServer.cid" if it exists
			{
				char pid_buf[16]; // declaring char buffer for reading PID
				len = read(fd, pid_buf, 16); // reading PID to "pid_buf" and count read bytes in "len" variable from "fd" file 
				pid_buf[len] = 0;  // input a 0-byte to buffer char array.
				pid = atoi(pid_buf); // converting char read from file to int
				if (pid != 0)
				{
					if (kill(pid, SIGTERM) != 0)
					{
						std::cout << "\nCannot close the proccess!!!\n"; // close the proccess with PID written in file
						exit(-1);// close programm with code "-1"
					}

					if (remove("/var/run/Server.pid") != 0) std::cout << "\nCannot remove CID file!!!"; // delete the PID file
					std::cout << "\nServer has been stopped(Daemon mode)\n";	// output a message to CLI clipboard
					exit(0); // close programm with code "0"
				}
				else
				{
					std::cout << "\nProblems with covertation - cannot stop the proccess 0\n";	// output a message to CLI clipboard
				}
			}
			else // if PID file doesn't exist
			{
				std::cout << "\nPID file not found. Cannot stop server.\n";	// output a message to CLI clipboard
				exit(-1); // close programm with code "-1"
			}
		}
		else
		{
			std::cout << "\nInvalid argument " << argv[1] << "\nInput '-i' to run chat-server in interface mode or '-d' to run in daemon mode\n";
			exit(-1); // close programm with code "-1"
		}

	}



}