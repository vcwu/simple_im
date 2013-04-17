/*
 * Project 2
 * CS 423
 * victoria wu
 * spring 2013*
 *
 *
 * Peer to Peer programming, connection oriented client/server
 * 
 * Main thread.
 * 	Creates socket for listening to peer requests and connecting to server.
 * 	Starts up thread to handle listening to server and peer requests.
 * 	Interacts with user.
 
 *	TODO:
 *		Pad port msg with ZEROES
 
 */

#define DEBUG
#define COTTER



#include <iostream>
#include <string>	//getline

//User defined includes
#include "Im_client.h"
//#include "IM_message.h"

#define WSVERS MAKEWORD(2,0)	//Win sock version.
#define BACKLOG 5
//#define LISTEN_PORT "65501"
//Compiled using Visual Studio's command prompt
#pragma comment(lib, "wsock32.lib")	//link winsock li
#pragma comment(lib, "libcmt.lib")	//for process.h


int main(int argc, char **argv)	{

	
	std::string LISTEN_PORT = "65501";

	//Take in user specified port num, server name.
	std::string serverName;
	std::string portNum;

	if(argc == 3)	{
		serverName = argv[1];
		portNum = argv[2];
		#ifdef DEBUG
		std::cout << "Server ip: " << serverName << "portNum: " <<
			portNum <<std::endl;
		std::cout << "Listening port : " << LISTEN_PORT << std::endl;
		#endif 
	}
	else if(argc == 2)	{
		LISTEN_PORT = argv[1];
		#ifdef COTTER
		serverName = "134.193.128.197";
		portNum = "3456";
		std::cout << "Listening port : " << LISTEN_PORT << std::endl;
		#endif
	}
	else	{
		#ifdef COTTER
		serverName = "134.193.128.197";
		portNum = "3456";
		#endif

		#ifndef DEBUG
		std::cerr << "Need server name and port num. ";
		exit(1);
		#endif
	}

	//WSA STARTUP
	WSADATA wsadata;
	if(WSAStartup(WSVERS, &wsadata) !=0 )	{
		std::cerr << "WSA startup failed " << std::endl;
		exit(1); 
	}
		
	Im_client client;
	client.startup(BACKLOG, serverName, portNum, LISTEN_PORT);
	
	//Log on to server.
	std::string name;
	std::cout << "What is your username? ";
	std::cin >> name;

	client.logOn(name, LISTEN_PORT);

	
	std::string input = " ";
	char command = ' ' ;
	bool userContinue = true;
	while(userContinue)	{
		client.displayMenu();
/*		while(input.length() < 2)	{
			getline(std::cin, input);
		}
*/
		getline(std::cin, input);
		(input.length() > 0) ? command = input.at(0) : command = 'X';
		switch (command)	{
			case 's': client.sendMessage();	break;
			case 'f': client.getFileNames(); break;
			case 'd': client.downloadFile(); break;
			case 'q': userContinue = false; break;
			default: std::cout << "Invalid input. " << std::endl;
				 printf("Input: %c\n", command);
		}
	}
	
	std::cout << " Bye! " << std::endl;
	client.shutdown();
	std::cout << "Finished shutting down. Please press any key to exit. " << std::endl;
	std::cin >> name;

	WSACleanup();
}
