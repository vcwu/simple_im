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
#include "MySock.h"

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
		printf("Server IP:%s\nPortNum:%s", serverName, portNum);
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

	//Start listening for peer requests.
	MySock peerListener("tcp");
	peerListener.startListening(BACKLOG);

	//Connect to server, start listening for server msgs.
	MySock serverListener("tcp");
	serverListener.connectToHost(serverName, portNum);
	//Log on to server.
	//GUI loop
	

	WSACleanup();
}
