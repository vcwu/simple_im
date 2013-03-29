/*
 * Project 2
 * CS 423
 * victoria wu
 * spring 2013*
 *
 * Peer to Peer programming, connection oriented client/server
 * 
 * Main thread.
 * 	Creates socket for listening to peer requests and connecting to server.
 * 	Starts up thread to handle listening to server and peer requests.
 * 	Interacts with user. 
 
 */

#define DEBUG

#include <iostream>

//User defined includes
#include "Im_client.h"
//#include "IM_message.h"

#define WSVERS MAKEWORD(2,0)	//Win sock version.
#define BACKLOG 5

//Compiled using Visual Studio's command prompt
#pragma comment(lib, "wsock32.lib")	//link winsock li
#pragma comment(lib, "libcmt.lib")	//for process.h

int main(int argc, char **argv)	{

	//Take in user specified port num, server name.
	std::string serverName;
	std::string portNum;

	if(argc == 3)	{
		serverName = argv[1];
		portNum = argv[2];
		#ifdef DEBUG
		std::cout << "Server ip: " << serverName << "portNum: " <<
			portNum <<std::endl;
		#endif 
	}
	else	{
		#ifdef DEBUG
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
	client.startup(BACKLOG, serverName, portNum);
	
	//Log on to server.
	std::string name;
	std::cout << "Username: " << std::endl;
	std::cin >> name;
	//client.logOn(name);

	std::cout << "YAY I LOGGED ON" <<std::endl;
	//GUI loop
	

	WSACleanup();
}
